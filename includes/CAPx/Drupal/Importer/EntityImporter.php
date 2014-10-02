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

    $options = $this->getOptions();
    $mapper = $this->getMapper();
    $client = $this->getClient();
    $data = array();

    foreach ($options['types'] as $k => $type) {
      $children = FALSE;

      switch ($type) {
        case "orgCodes":
          $children = $options['child_orgs'];
        case "privGroups":
        case "uids":
          $new = $client->api('profile')->search($type, $options['values'][$k], FALSE, $children);
          if (!empty($new['values'])) {
            $data[$type] = $new['values'];
          }
          break;
      }
    }

    if (!empty($data)) {
      foreach ($data as $type => $results) {
        foreach ($results as $index => $info) {
          drupal_alter('capx_pre_entity_processor', $info, $mapper);

          $entityType = $mapper->getEntityType();
          $entityType = ucfirst(strtolower($entityType));
          $className = "\CAPx\Drupal\Processors\\" . $entityType . 'Processor';

          if (class_exists($className)) {
            $processor = new $className($mapper, $info);
            $processor->setEntityImporter($this);
            $processor->execute();
          }
          else {
            $processor = new EntityProcessor($mapper, $info);
            $processor->setEntityImporter($this);
            $processor->execute();
          }
        }
      }
    }

  }

  // ===========================================================================
  // GETTERS & SETTERS
  // ===========================================================================

  /**
   * Getter function
   * @return array an arry of options
   */
  protected function getOptions() {
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
