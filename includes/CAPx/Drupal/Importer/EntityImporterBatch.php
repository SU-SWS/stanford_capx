<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Processors\EntityProcessor;
use CAPx\Drupal\Util\CAPxImporter;

/**
 * A static class for handling Batch/Queue API callbacks.
 */
class EntityImporterBatch {


  /**
   * Callback for batch import functionality.
   * @param  [type] $type         [description]
   * @param  [type] $importerMachineName [description]
   * @param  [type] $page         [description]
   * @param  [type] $limit        [description]
   * @param  [type] $context      [description]
   * @return [type]               [description]
   */
  public static function batch($type, $importerMachineName, $page, $limit, &$context) {

    // Define a lot of things...
    $importer = CAPxImporter::loadEntityImporter($importerMachineName);
    $options = $importer->getOptions();
    $children = $options['child_orgs'];
    $mapper = $importer->getMapper();
    $client = $importer->getClient();
    $types = $options['types'];

    // We need to adjust the search to grab results from the correct page.
    $httpOpts = $client->getHttpOptions();
    $httpOpts['query']['p'] = $page;
    $httpOpts['query']['ps'] = $limit;
    $client->setHttpOptions($httpOpts);

    // In order to get the values to search for we need to find out what index
    // the type is as the values are in the corresponding index.
    $index = array_search($type, $types);
    $search = $options['values'][$index];

    $response = $client->api('profile')->search($type, $search, FALSE, $children);
    $results = $response['values'];

    EntityImporterBatch::processResults($results, $importer);

    $now = time();
    $importer->getImporter()->setLastCronRun($now);
  }

  /**
   * Callback function for cron queue processing.
   * @param  [type] $item [description]
   * @return [type]       [description]
   */
  public static function queue($item) {

    // Define a lot of things...
    $type = $item['type'];
    $importerMachineName = $item['importer'];
    $page = $item['page'];
    $limit = $item['limit'];
    $importer = CAPxImporter::loadEntityImporter($importerMachineName);
    $options = $importer->getOptions();
    $children = $options['child_orgs'];
    $client = $importer->getClient();
    $types = $options['types'];

    // We need to adjust the search to grab results from the correct page.
    $httpOpts = $client->getHttpOptions();
    $httpOpts['query']['p'] = $page;
    $httpOpts['query']['ps'] = $limit;
    $client->setHttpOptions($httpOpts);

    // In order to get the values to search for we need to find out what index
    // the type is as the values are in the corresponding index.
    $index = array_search($type, $types);
    $search = $options['values'][$index];

    $response = $client->api('profile')->search($type, $search, FALSE, $children);
    $results = $response['values'];

    EntityImporterBatch::processResults($results, $importer);

    $now = time();
    $importer->getImporter()->setLastCronRun($now);

  }

  /**
   * Process the results from the response from the API.
   * @param  [type] $results  [description]
   * @param  [type] $importer [description]
   * @return [type]           [description]
   */
  public static function processResults($results, $importer) {

    $mapper = $importer->getMapper();

    // Allow altering of the results.
    drupal_alter('stanford_capx_preprocess_results', $results, $importer);

    // Loop through each result (profile info) and send it to the processor for
    // mapping and saving.
    foreach ($results as $index => $info) {

      // Only one type of processor for now. Plan to add other types in the
      // future.
      $processor = new EntityProcessor($mapper, $info);
      $processor->setEntityImporter($importer);
      $processor->execute();

      // Log some information. This needs to be better.
      watchdog('stanford_capx', 'Synced: ' . $info['displayName'], array(), WATCHDOG_DEBUG);
      if (function_exists('drush_log')) {
        drush_log('Synced: ' . $info['displayName'], 'ok');
      }

    }

  }

}
