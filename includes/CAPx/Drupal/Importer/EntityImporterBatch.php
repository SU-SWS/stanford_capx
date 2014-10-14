<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Organizations\Orgs;
use CAPx\Drupal\Processors\EntityProcessor;
use CAPx\Drupal\Util\CAPxImporter;
/**
 * A static class for handling Batch/Queue API callbacks.
 */
class EntityImporterBatch {


  /**
   * Callback for batch import functionality.
   * @param string $type
   *   The type of import bing executed (orgcodes, workgroup, sunets)
   * @param string $importerMachineName
   *   The machine name of the importer configuration entity.
   * @param int $page
   *   The page of results to process
   * @param int $limit
   *   The limit of results per page to process
   * @param array $context
   *   Batch context information passed by reference.
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

    $success = EntityImporterBatch::processResults($results, $importer);

    if ($success) {
      $now = time();
      $importer->setLastCronRun($now);
    }

  }

  /**
   * Callback function for cron queue processing.
   *
   * Fetches and parses results from the CAP API server based on settings from
   * the item array that is being passed in. This function loads up fresh
   * configuration from the importer and mapper so it is possible that things
   * have changed since the queue item was established.
   *
   * @param array $item
   *   An array of information to use during the queue call.
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

    $success = EntityImporterBatch::processResults($results, $importer);

    if ($success) {
      $now = time();
      $importer->setLastCronRun($now);
    }

  }

  /**
   * Process the results from the response from the API.
   *
   * This function handles the values that the CAP API server has returned from
   * either the batch or queue processes.
   *
   * @param array $results
   *   An array of profile information to process
   * @param EntityImporter $importer
   *   The EntityImporter object that is currently importing the profiles.
   *
   * @return bool
   *   success status.
   */
  public static function processResults($results, $importer) {

    // No results.
    if (empty($results)) {
      return FALSE;
    }

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

    return TRUE;
  }

  /**
   * A queue callback function that looks for orphaned profiles.
   *
   * Profiles that have been downloaded will need to be compared against each
   * of the type conditions. This causes three queries to the server per
   * orphan check but I don't see a way out of this yet.
   *
   * Removed from CAP orphan (sunet id check)
   *
   * 1. Gather up all the ideas that are attached to this importer
   * 2. Break them up into the same number of groups as batch does
   * 3. Fetch the profiles by their profile ids
   * 4. Compare results to list of local.
   *
   * Removed from an organization orphan (org check)
   *
   * 1. Gather up all the ids that are attached to this importer
   * 2. Break them up into the same number of groups as batch does
   * 3. Fetch the profiles by their profile ids
   * 4. Look through the results for the organizations and grab the orgCodes
   * 5. Compare org codes with org code settings through vocabulary tree.
   *
   * Removed from a workgroup orphan (workgroup check)
   *
   * 1. Gather up all the ids that are attached to this importer
   * 2. Break them up into the same number of groups as batch does
   * 3. Fetch all results from a workgroup by fetching a large number limit
   * 4. Compare results to set.
   *
   * If profile id is missing from all three of these checks then it is deemed
   * a true orphan and should be actioned on as such.
   *
   * @param array $item
   *   An array of information to be used to find the orphans.
   *   $item['importer'] = "importer_machine_name"
   *   $item['profiles'] = array(profile_id, profile_id, profile_id)
   */
  public static function orphans($item) {

    $importer =  CAPxImporter::loadEntityImporter($item['importer']);
    $options = $importer->getOptions();
    $profiles = $item['profiles'];
    $client = $importer->getClient();
    $limit = count($profiles);
    $orphans = array();

    // We need to adjust the search to grab all the results.
    $httpOpts = $client->getHttpOptions();
    $httpOpts['query']['ps'] = $limit;
    $client->setHttpOptions($httpOpts);

    // Make one request to the server for the profile information.
    $response = $client->api('profile')->search("ids", $profiles);
    $results = $response['values'];

    // If SuNet IDs is an option we want to run that first as it is the least
    // processor intensive and we can immediately call out the profile as an
    // orphan if the API does not return a value.

    if (isset($options['ids'])) {
      $ids = explode(",", $options['sunet_id']);
      $orphans["ids"] = EntityImporterBatch::findSuNetOrphans($results, $ids);
    }

    foreach ($options['types'] as $type) {

      // Already processed above.
      if ($type == "ids") {
        continue;
      }

      // Organization orphans.
      if ($type == "orgCodes") {
        $codes = explode(",", $options['organization']);
        $orphans[$type] = EntityImporterBatch::findOrgCodeOrphans($results, $codes);
      }

       // Workgroup orphans.
      if ($type == "privGroups") {
        $groups = explode(",", $options['workgroup']);
        $orphans[$type] = EntityImporterBatch::findWorkgroupOrphans($results, $groups);
      }

    }

    // If we only have one option available then we can be assured that the
    // profile is an orphan.
    if (count($options['types']) == 1 && !empty($orphans)) {
      $process = array_pop($orphans);
      EntityImporterBatch::ProcessOrphans($process, $importer);
      return;
    }

    // If a profile was marked as orphaned and there is two or more ways to get
    // profiles, it has to be marked as an orphan in all ways.

    // @todo: once you have completed the workgroup find orphans option you will
    // then need to complete this next section where you will loop through all
    // of the orphaned results and compare profile ids. If you have a match
    // across all import options then you have a profile that is orphaned.

  }

  /**
   * @param array $results
   *   A result set from the API server
   * @param array $codes
   *   An array or organization codes the importer is importing.
   *
   * @return array
   *   An array of orphaned profile ids
   */
  public static function findOrgCodeOrphans($results, $codes) {
    $orphans = array();

    // Storage for the codes we will need to look up.
    $keyTids = array();

    // Get any and all aliases of the importer orgCodes.
    $aliases = Orgs::getAliasesByCode($codes);
    $codes = array_merge($codes, $aliases);

    // Load up the tids of the orgs that the importer is importing.
    foreach ($codes as $code) {
      $terms = taxonomy_get_term_by_name($code, Orgs::getVocabularyMachineName());
      $term = array_pop($terms);
      $keyTids[] = $term->tid;
    }

    // Look through the org codes attached to each profile.
    foreach ($results as $k => $profile) {
      $found = FALSE;
      foreach ($profile['titles'] as $title) {
        $orgCode = $title['organization']['orgCode'];
        $org = array_pop(taxonomy_get_term_by_name($orgCode, Orgs::getVocabularyMachineName()));

        // If the org itself is in the list.
        if (in_array($org->tid, $keyTids)) {
          $found = TRUE;
        }

        // Check if any of the parents orgs codes match one in the keyTids.
        // If there is a match then this org code is a child of one of the key
        // importer codes.
        $parents = taxonomy_get_parents_all($org->tid);
        if (!empty($parents) && !$found) {
          foreach ($parents as $parentTerm) {
            if (in_array($parentTerm->tid, $keyTids)) {
              $found = TRUE;
            }
          }
        }

      }

      // If we did not find an organization.
      if (!$found) {
        $orphans[$profile['uid']] = $profile['profileId'];
      }

    }

    return $orphans;
  }

  /**
   * [findWorkgroupOrphans description]
   * @param  [type] $results [description]
   * @param  [type] $groups  [description]
   * @return [type]          [description]
   */
  public static function findWorkgroupOrphans($results, $groups) {
    $orphans = array();

    return $orphans;
  }

  /**
   * [findSunetOrphans description]
   * @param  [type] $results [description]
   * @param  [type] $ids     [description]
   * @return [type]          [description]
   */
  public static function findSunetOrphans(&$results, $ids) {
    $orphans = array();
    foreach ($results as $index => $profile) {
      if (!in_array($profile['uid'], $ids)) {
        $orphans[$profile['uid']] = $profile['profileId'];

        // If we determine that a profile is an orphan this way we should remove
        // it from the array so that there is no action on a later process.
        unset($results[$index]);
      }
    }
    return $orphans;
  }


  /**
   * Handles what to do when a profile has been orphaned.
   *
   * @param [type] $profileIds [description]
   */
  public static function ProcessOrphans($profileIds, $importer) {
    // @todo: yes, this.
  }

}
