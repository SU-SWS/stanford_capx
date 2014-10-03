<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Processors\EntityProcessor;
use CAPx\Drupal\Util\CAPxImporter;

/**
 * A static class for handling Batch API callbacks.
 */
class EntityImporterBatch {

  /**
   * Callback for batch function processing.
   * @param  [type] $client  [description]
   * @param  [type] $options [description]
   * @param  [type] $type    [description]
   * @param  [type] $page    [description]
   * @param  [type] $limit   [description]
   * @param  [type] $context [description]
   * @return [type]          [description]
   */
  public static function batch($client, $options, $type, $page, $limit, &$context) {
    drupal_set_message("Running: " . $type . " | Page: " . $page, 'status');
    $context['message'] = t('Now processing page %page - %type:', array('%page' => $page, '%type' => $type));
  }

  /**
   * Callback function for cron queue processing.
   * @param  [type] $item [description]
   * @return [type]       [description]
   */
  public static function queue($item) {

    $type = $item['type'];
    $importerMachineName = $item['importer'];
    $page = $item['page'];
    $limit = $item['limit'];
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

    drush_log("FOUND: " . count($results), 'status');

    drupal_alter('stanford_capx_preprocess_results', $results, $importer);

    foreach ($results as $index => $info) {

      $processor = new EntityProcessor($mapper, $info);
      $processor->setEntityImporter($importer);
      $processor->execute();

      watchdog('stanford_capx', 'Synced: ' . $info['displayName'], array(), WATCHDOG_DEBUG);
      if (function_exists('drush_log')) {
        drush_log('Synced: ' . $info['displayName'], 'ok');
      }

    }

  }

}
