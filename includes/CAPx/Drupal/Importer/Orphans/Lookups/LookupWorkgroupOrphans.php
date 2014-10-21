<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer\Orphans\Lookups;

class LookupWorkgroupOrphans implements LookupInterface {

  /**
   * Look through the workgroups for the profile results.
   *
   * @todo This function has the potential to send & receive a very large
   * request to/from the server and could be a potential breaking point. Need to
   * revisit this in order to break it out into a smaller check somehow.
   *
   * @param EntityImporterOrphans $orphaner
   *   The orphan processor object.
   *
   * @return array
   *   The remaining orphans
   */
  public function execute($orphaner) {

    $options = $orphaner->getImporterOptions();
    $orphans = $orphaner->getOrphans();

    // Check to see if there is something to run on.
    if (!in_array("privGroups", $options['types'])) {
      return $orphans;
    }

    $client = $orphaner->getClient();
    $profiles = $orphaner->getProfiles();
    $groups = explode(",", $options['workgroup']);

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
    $found = array_flip($profiles);

    if (!empty($found)) {
      $orphans['privGroups'] = $found;
    }

    return $orphans;
  }

}
