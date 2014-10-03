<?php
/**
 * @file
 * Entity importer class handles the execution process which fires off the
 * API request, parses the data, and sends it to an entity processor.
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Util\CAPx;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Processors\EntityProcessor as EntityProcessor;
use CAPx\Drupal\Processors\UserProcessor as UserProcessor;
use CAPx\APILib\HTTPClient;


class EntityImporter implements ImporterInterface {

  // Options and configuration array
  protected $options = array();

  // The mapping scheme object
  protected $mapper;

  // The machine name of the CFE Importer entity.
  protected $machineName = '';

  /**
   * Constructor class. Sets a number of items.
   * @param [type] $config [description]
   */
  public function __construct(Array $config, EntityMapper $mapper, HTTPClient $client) {
    $this->addOptions($config);
    $this->setMapper($mapper);
    $this->setClient($client);
    $this->setMachineName($config['machine_name']);
  }

  /**
   * Run the show!
   * This function is the entry point to importing the whole thing.
   */
  public function execute() {

    // $options = $this->getOptions();
    // $mapper = $this->getMapper();
    // $client = $this->getClient();
    // $data = array();

    // foreach ($options['types'] as $k => $type) {
    //   $children = FALSE;

    //   switch ($type) {
    //     case "orgCodes":
    //       $children = $options['child_orgs'];
    //     case "privGroups":
    //     case "uids":
    //       $new = $client->api('profile')->search($type, $options['values'][$k], FALSE, $children);
    //       if (!empty($new['values'])) {
    //         $data[$type] = $new['values'];
    //       }
    //       break;
    //   }
    // }

    // if (!empty($data)) {
    //   foreach ($data as $type => $results) {
    //     foreach ($results as $index => $info) {
    //       drupal_alter('capx_pre_entity_processor', $info, $mapper);

    //       $entityType = $mapper->getEntityType();
    //       $entityType = ucfirst(strtolower($entityType));
    //       $className = "\CAPx\Drupal\Processors\\" . $entityType . 'Processor';

    //       if (class_exists($className)) {
    //         $processor = new $className($mapper, $info);
    //         $processor->setEntityImporter($this);
    //         $processor->execute();
    //       }
    //       else {
    //         $processor = new EntityProcessor($mapper, $info);
    //         $processor->setEntityImporter($this);
    //         $processor->execute();
    //       }
    //     }
    //   }
    // }

  }

  /**
   * Executes the import process as a series of batch processors. First pings
   * the server for each type and finds out how many results there are. It then
   * breaks up the results into sensible batch sizes.
   * @return [type] [description]
   */
  public function createBatch() {

    $options = $this->getOptions();
    $client = $this->getClient();
    $responses = array();

    // Loop through each of the import type options and ping the server for just
    // one item of each to get an idea of how many items there actually is.
    foreach ($options['types'] as $k => $type) {

      $children = FALSE;
      switch ($type) {
        case "orgCodes":
          $children = $options['child_orgs'];
        case "privGroups":
        case "uids":

          // Set the results to one per page.
          $httpOptions = $client->getHttpOptions();
          $httpOptions['query']['ps'] = 1;
          $client->setHttpOptions($httpOptions);

          // Fire off request
          $results = $client->api('profile')->search($type, $options['values'][$k], FALSE, $children);
          break;
      }

      // Keep a track of the number of items.
      $responses[$type] = array();
      $responses[$type]['totalCount'] = $results['totalCount'];

    }

    // How many to run per batch
    $processLimit = variable_get('stanford_capx_batch_limit', 50);

    // Batch definition
    $batch = array(
      'operations' => array(),
      'title' => t('Downloading and processing profile information...'),
      'init_message' => t('Profile information sync is starting.'),
      'progress_message' => t('Profile sync in progress. @current of @total completed.'),
      'error_message' => t('Profile information could not be imported. Please try again.'),
    );

    // Loop through each type and chunk up into patch operations.
    foreach ($responses as $type => $info) {
      $count = $info['totalCount'];
      $i = 0;
      while($count > 0) {
        $i++;
        $batch['operations'][] = array('\CAPx\Drupal\Importer\EntityImporterBatch::callback', array($client, $options, $type, $i, $processLimit));
        $count -= $processLimit;
      }
    }

    // Set the big batch after all...
    batch_set($batch);
  }

  /**
   * Create a list of queued items that need to be ran on cron.
   * @see  Queues API.
   * @return [type] [description]
   */
  public function createQueue() {

    $queue = \DrupalQueue::get('stanford_capx_profiles', TRUE); // should be reliable?
    $options = $this->getOptions();
    $client = $this->getClient();
    $limit = variable_get('stanford_capx_batch_limit', 50);

    // Loop through each of the import type options and ping the server for just
    // one item of each to get an idea of how many items there actually is.
    // Break up the number of items into sensible chunks.

    foreach ($options['types'] as $k => $type) {

      $children = FALSE;
      switch ($type) {
        case "orgCodes":
          $children = $options['child_orgs'];
        case "privGroups":
        case "uids":

          // Set the results to one per page.
          $httpOptions = $client->getHttpOptions();
          $httpOptions['query']['ps'] = 1;
          $client->setHttpOptions($httpOptions);

          // Fire off request
          $results = $client->api('profile')->search($type, $options['values'][$k], FALSE, $children);
          break;
      }

      // Total number of profiles.
      $total = $results['totalCount'];
      $page = 1;

      // Create queues for each page
      while ($total > 0) {
        $item = $this->getQueueItem();
        $item['type'] = $type;
        $item['page'] = $page;
        $queue->createItem($item);
        $page++;
        $total -= $limit;
      }

    }

  }

  // ===========================================================================
  // GETTERS & SETTERS
  // ===========================================================================

  /**
   * A template item function. Returns the default options for an item that
   * is going to go into the Queues API.
   * @return array a keyed array with values that need to be passed to the queue
   */
  protected function getQueueItem() {
    $limit = variable_get('stanford_capx_batch_limit', 50);

    // Each queue needs some items. Here is a template for that.
    $item = array(
      'type' => '', // The import type (sunet, orgCodes, workgroup)
      'page' => 1, // The page on the API to import
      'limit' => $limit, // The limit per page
      'importer' => $this->getMachineName(), // This name so we can load er up.
    );

    return $item;
  }

  /**
   * Getter function
   * @return array an arry of options
   */
  public function getOptions() {
    return $this->options;
  }

  /**
   * Adder function
   * @param array - Adds an array of options into the already defined options.
   */
  public function addOptions($newOpts) {
    $opts = $this->getOptions();
    $opts = array_merge($opts, $newOpts);
    $this->setOptions($opts);
  }

  /**
   * Setter function
   * @param array - an array of options
   */
  protected function setOptions($opts) {
    $this->options = $opts;
  }

  /**
   * Getter function
   * @return EntityMapper an EntityMapper instance.
   */
  public function getMapper() {
    return $this->mapper;
  }

  /**
   * Setter function
   * @param EntityMapper - an EntityMapper instance
   */
  public function setMapper($map) {
    $this->mapper = $map;
  }

  /**
   * Getter function
   * @return HTTPClient the HTTPClient instance.
   */
  public function getClient() {
    return $this->client;
  }

  /**
   * Setter function
   * @param HTTPClient the HTTPClient instance.
   */
  public function setClient($client) {
    $this->client = $client;
  }

  /**
   * Getter function
   * @return string The machine name of the importer configuration entity.
   */
  public function getMachineName() {
    return $this->machineName;
  }

  /**
   * Setter function
   * @param string $name the machine name of the importer configuration entity.
   */
  public function setMachineName($name) {
    $this->machineName = $name;
  }

}
