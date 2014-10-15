<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Organizations\Orgs;
use CAPx\Drupal\Util\CAPx;
use CAPx\Drupal\Util\CAPxImporter;

class EntityImporterOrphans {

  /**
   * A queue callback function that looks for orphaned profiles.
   *
   * Profiles that have been downloaded will need to be compared against each
   * of the type conditions.
   *
   * Removed from CAP orphan (sunet id check)
   *
   * 1. Gather up all the ids that are attached to this importer
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
    $client->setLimit($limit);

    // Make one request to the server for the profile information to check for
    // profiles that no longer exist on the API.
    $response = $client->api('profile')->search("ids", $profiles);
    $results = $response['values'];
    // Allow those to alter this set.
    drupal_alter('capx_orphan_profile_results', $results);


    // Always check to see if a profile is plain jane just missing.
    $found = EntityImporterOrphans::findSuNetOrphansByServer($results, $profiles);
    if (!empty($found)) {
      $orphans["missing"] = $found;
    }

    // Check to see if we have some saved profiles that are now no longer in the
    // sunet id options.
    if (in_array('uids', $options['types'])) {
      $ids = explode(",", $options['sunet_id']);
      $found = EntityImporterOrphans::findSuNetOrphansByLocal($results, $profiles, $ids);
      if (!empty($found)) {
        $orphans["uids"] = $found;
      }
    }

    // Organization orphans.
    if (in_array("orgCodes", $options['types'])) {
      $codes = explode(",", $options['organization']);
      $found = EntityImporterOrphans::findOrgCodeOrphans($results, $codes);
      if (!empty($found)) {
        $orphans["orgCodes"] = $found;
      }
    }

    // Workgroup orphans.
    if (in_array("privGroups", $options['types'])) {
      $groups = explode(",", $options['workgroup']);
      $found = EntityImporterOrphans::findWorkgroupOrphans($profiles, $groups, $client);
      if (!empty($found)) {
        $orphans["privGroups"] = $found;
      }
    }

    // If we have more than one option enabled we have to do some more logic.
    // Start by going through the sunet ids. If there are any ids that have been
    // marked as orphan we can safely call it an orphan.
    if (!empty($orphans['missing'])) {
      EntityImporterOrphans::ProcessOrphans($orphans['missing'], $importer);
      unset($orphans['missing']);
    }

    // If we only have one option available then we can be assured that the
    // profile is an orphan.
    if (count($options['types']) == 1 && !empty($orphans)) {
      $process = array_pop($orphans);
      EntityImporterOrphans::ProcessOrphans($process, $importer);
      return;
    }

    // At this point we can be sure that $results exists as there has to be more
    // than one option enabled and the logic says to set $results if there is
    // two or more ways of importing profiles.

    // Comparison scenarions. If there are orphans in some they must be checked
    // against the others.
    EntityImporterOrphans::CompareOrphansSunetOrgCodes($orphans, $results, $options);
    EntityImporterOrphans::CompareOrphansSunetWorkgroups($orphans, $results, $options);
    EntityImporterOrphans::CompareOrphansOrgCodesWorkgroups($orphans, $results, $options);
    EntityImporterOrphans::CompareOrphansOrgCodesSunet($orphans, $results, $profiles, $options);
    EntityImporterOrphans::CompareOrphansWorkgroupsOrgCodes($orphans, $results, $options);
    EntityImporterOrphans::CompareOrphansWorkgroupsSunet($orphans, $results, $profiles, $options);

    // If we have no orphans after all of that just end.
    if (empty($orphans)) {
      return;
    }

    // We have looked at everything and now it is time to process the orphans.
    // In order to be an orphan the orphan id has to appear in all importer
    // Options. So we can just take one and run the process on that.

    $orphaned = array_pop($orphans);
    EntityImporterOrphans::ProcessOrphans($orphaned, $importer);

  }

  /**
   * Find orphans from an Organization.
   *
   * Loop through a number of profileIds and look up their Organization relation
   * to see if they are still in the Org or Org tree if children are also being
   * imported. Org code aliases should also be included in this lookup.
   *
   * @param array $results
   *   A result set from the API server
   * @param array $codes
   *   An array or organization codes the importer is importing.
   *
   * @return array
   *   An array of orphaned profile ids
   */
  protected static function findOrgCodeOrphans($results, $codes) {
    $orphans = array();

    // Storage for the codes we will need to look up.
    $keyTids = array();

    // Get any and all aliases of the importer orgCodes.
    $aliases = Orgs::getAliasesByCode($codes);
    $codes = array_merge($codes, $aliases);

    // Allow those to alter the codes.
    drupal_alter("capx_find_org_orphans_codes", $codes);

    // Load up the tids of the orgs that the importer is importing.
    foreach ($codes as $code) {
      $terms = taxonomy_get_term_by_name($code, Orgs::getVocabularyMachineName());
      $term = array_pop($terms);
      $keyTids[] = $term->tid;
    }

    // Look through the org codes attached to each profile.
    foreach ($results as $k => $profile) {
      $found = FALSE;

      // If there are no titles for the profile then they cannot be in an org.
      if (!isset($profile['titles'])) {
        $orphans[$profile['uid']] = $profile['profileId'];
        continue;
      }

      foreach ($profile['titles'] as $title) {

        $orgCode = $title['organization']['orgCode'];
        $org = array_pop(taxonomy_get_term_by_name($orgCode, Orgs::getVocabularyMachineName()));

        // If the org code itself is in the list.
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

      // If we did not find an organization that matches we have an orphan.
      if (!$found) {
        $orphans[$profile['uid']] = $profile['profileId'];
      }

    }

    // Allow those to alter this set.
    drupal_alter("capx_find_org_orphans", $orphans);
    return $orphans;
  }

  /**
   * Look through the workgroups for the profile results.
   *
   * @todo This function has the potential to send & receive a very large
   * request to/from the server and could be a potential breaking point. Need to
   * revisit this in order to break it out into a smaller check somehow.
   *
   * @param array $profiles
   *   An array of profile ids that have been imported by this importer.
   * @param array $groups
   *   An array of workgroups (string)
   *
   * @return array
   *   An array of profile ids that have been orphaned or an empty array.
   */
  protected static function findWorkgroupOrphans($profiles, $groups, $client) {
    drupal_alter("capx_find_workgroup_orphans_groups", $groups);

    // Flip the profileIds so that the profile array is keyed.
    $profiles = array_flip($profiles);

    // Workgroup profile information is only available if you look up a
    // workgroup directly. Unfortunately, that means that we need to make
    // another request to the server for each work group.

    $client->setLimit(99999);
    $response = $client->api('profile')->search("privGroups", $groups);
    $results = $response['values'];

    drupal_alter('capx_orphan_profile_results', $results);

    // Loop through the results and unset the profiles from the passed in list.
    foreach ($results as $index => $profile) {
      unset($profiles[$profile['profileId']]);
    }

    // If we have any left over we have profiles that are not in this workgroup.
    $orphans = array_flip($profiles);

    drupal_alter("capx_find_workgroup_orphans", $orphans);
    return $orphans;
  }

  /**
   * Checks the API server to see if the SUNET item still exists.
   *
   * @param  [type] $results [description]
   * @param  [type] $ids     [description]
   * @return [type]          [description]
   */
  protected static function findSunetOrphansByServer($results, $profiles) {
    $resultIds = array();

    // Gather up the profileIds of the request results.
    foreach ($results as $index => $profile) {
      $resultIds[] = $profile['profileId'];
    }

    // Compare the results to the profiles we are looking for
    $orphans = array_diff($profiles, $resultIds);

    // Allow alter.
    drupal_alter("capx_find_sunet_orphans", $orphans);
    return $orphans;
  }

  /**
   * @param $profiles
   * @param $ids
   *
   * @return array
   */
  protected static function findSunetOrphansByLocal($results, $profiles, $ids) {
    $orphans = array();

    $sunetIds = array();
    foreach ($results as $index => $profile) {
      $sunetIds[$profile['profileId']] = $profile['uid'];
    }

    // Loop through the profiles looking for ids. If we cannot find one in the
    // list of ids then we have an orphan.
    foreach ($profiles as $index => $profileId) {
      if (!in_array($sunetIds[$profileId], $ids)) {
        $orphans[$sunetIds[$profileId]] = $profileId;
      }
    }

    return $orphans;
  }

  /**
   * Compare sunet orphans with orgcode orphans.
   *
   * Check to see if our missing sunet options are in an org group by checking
   * to see if the org group has the same missing profile id.
   * @param [type] $orphans [description]
   * @param [type] $results [description]
   * @param [type] $options [description]
   */
  protected static function CompareOrphansSunetOrgCodes(&$orphans, $results, $options) {

    // If either orphan group is empty we cannot continue.
    if (empty($orphans['uids']) || empty($orphans['orgCodes'])) {
      return;
    }

    foreach ($orphans['uids'] as $index => $profileId) {
      if (!in_array($profileId, $orphans['orgCodes'])) {
        // Not an orphan.
        unset($orphans['uids'][$index]);
      }
    }

  }

  /**
   * Check to see if our missing sunet options are in a workgroup group by
   * checking to see if the org group has the same missing profile id.
   * @param [type] $orphans [description]
   * @param [type] $results [description]
   * @param [type] $options [description]
   */
  protected static function CompareOrphansSunetWorkgroups(&$orphans, $results, $options) {

    // If either orphan group is empty we cannot continue.
    if (empty($orphans['uids']) || empty($orphans['privGroups'])) {
      return;
    }

    foreach ($orphans['uids'] as $index => $profileId) {
      if (!in_array($profileId, $orphans['privGroups'])) {
        // Not an orphan.
        unset($orphans['uids'][$index]);
      }
    }

  }

  /**
   * Check to see if any of the orphans in the organization groups are in a
   * workgroup.
   *
   * @param [type] $orphans [description]
   * @param [type] $results [description]
   * @param [type] $options [description]
   */
  protected static function CompareOrphansOrgCodesWorkgroups(&$orphans, $results, $options) {
    // OrgCodes:
    // If both workgroup and organizations are selected we need to see that the
    // orphan is missing from both and the sunet is not in the ids.
    if (!empty($orphans['orgCodes'])) {
      foreach ($orphans['orgCodes'] as $index => $profileId) {

        // Scenario: Orphan in orgCodes but not privGroups.
        if (isset($orphans['privGroups']) && !in_array($profileId, $orphans['privGroups'])) {
          // Not an orphan. Remove it.
          unset($orphans['orgCodes'][$index]);
        }
      }
    }
  }

  /**
   * Check to see if any of the orphans in the workgroups are in an
   * organization.
   * @param [type] $orphans [description]
   * @param [type] $results [description]
   * @param [type] $options [description]
   */
  protected static function CompareOrphansWorkgroupsOrgCodes(&$orphans, $results, $options) {
    // Workgroups:
    // If both workgroup and organizations are selected we need to see that the
    // orphan is missing from both and the sunet is not in the ids.
    if (!empty($orphans['privGroups'])) {
      foreach ($orphans['privGroups'] as $index => $profileId) {
        // Scenario: Orphan in privGroups but not orgCodes.
        if (isset($orphans['orgCodes']) && !in_array($profileId, $orphans['orgCodes'])) {
          // Not an orphan. Remove it.
          unset($orphans['privGroups'][$index]);
        }
      }
    }
  }

  /**
   * Check to see if any of the missing organization profiles are in the sunet
   * option.
   * @param [type] $orphans [description]
   * @param [type] $results [description]
   * @param [type] $options [description]
   */
  protected function CompareOrphansOrgCodesSunet(&$orphans, $results, $profiles, $options) {

    $sunetIds = array();
    foreach ($results as $index => $profile) {
      $sunetIds[$profile['profileId']] = $profile['uid'];
    }

    $sunetOptions = explode(",", $options['sunet_id']);

    foreach ($orphans['orgCodes'] as $index => $profileId) {
      if (in_array($sunetIds[$profileId], $sunetOptions)) {
        // If the sunet code is found in the sunet id list option then the
        // profile is not an orphan and we need to unset it from the orphans.
        unset($orphans["orgCodes"][$index]);
      }
    }

  }


  /**
   * Check to see if any of the missing workgroup profiles are in the sunet
   * option.
   * @param [type] $orphans [description]
   * @param [type] $results [description]
   * @param [type] $options [description]
   */
  protected function CompareOrphansWorkgroupsSunet(&$orphans, $results, $profiles, $options) {
    $sunetIds = array();
    foreach ($results as $index => $profile) {
      $sunetIds[$profile['profileId']] = $profile['uid'];
    }

    $sunetOptions = explode(",", $options['sunet_id']);

    foreach ($orphans['privGroups'] as $index => $profileId) {
      if (in_array($sunetIds[$profileId], $sunetOptions)) {
        // If the sunet code is found in the sunet id list option then the
        // profile is not an orphan and we need to unset it from the orphans.
        unset($orphans["orgCodes"][$index]);
      }
    }

  }


  /**
   * Handles what to do when a profile has been orphaned.
   *
   * @param [type] $profileIds [description]
   */
  public static function ProcessOrphans($profileIds, $importer) {

    $entityType = $importer->getEntityType();
    $bundleType = $importer->getBundleType();
    $action = variable_get("stanford_capx_orphan_action", "unpublish");

    foreach ($profileIds as $id) {
      $profile = CAPx::getEntityByProfileId($entityType, $bundleType, $id);
      $profile = entity_metadata_wrapper($entityType, $profile);

      switch ($action) {
        case "delete":
          drush_log("Deleted orphaned profile: " . $profile->label(), "status");
          $profile->delete();
          break;

        case "unpublish":
          $profile->status->value(0);
          $profile->save();
          drush_log("Unpublished orphaned profile: " . $profile->label(), "status");
          break;
      }
    }

  }

}
