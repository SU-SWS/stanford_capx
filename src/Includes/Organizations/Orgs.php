<?php
/**
 * @file
 * Obtains Stanford organization codes and related information, and stores
 * them in the capx_organizations vocabulary
 */

namespace Drupal\stanford_capx\Includes\Organizations;

use Drupal\taxonomy\Entity\Vocabulary;

class Orgs {
  public static $vocabularyMachineName = "capx_organizations";


  /**
   * Gets and syncs the Organization data from the CAP API.
   *
   * @param bool $batch
   *   boolean value on wether or not to use batch api.
   */
  public static function syncOrganizations($batch = FALSE) {

    $vocab = Orgs::getVocabulary();

    /*
    $client = \CAPx\Drupal\Util\CAPxConnection::getAuthenticatedHTTPClient();
    $org = 'AA00';
    $orgInfo = $client->api('org')->getOrg($org);

    if (!$orgInfo) {
      throw new \Exception("Could not get organization information.");
    }

    Orgs::prepareVocabulary();

    // No batch.
    if (!$batch) {
      $termTree = Orgs::saveOrganizations($orgInfo);
      return;
    }

    // Batching.
    $batch = array(
      'operations' => array(
        array('\CAPx\Drupal\Organizations\Orgs::syncOrganizationsBatch', array($orgInfo)),
      ),
      'title' => t('Processing Organization Codes'),
      'init_message' => t('Organization codes sync is starting.'),
      'progress_message' => t('Syncing organization codes in progress.'),
      'error_message' => t('Organization codes could not be imported. Please try again.'),
      'finished' => 'stanford_capx_orgs_batch_finished',
    );

    batch_set($batch);
    batch_process(drupal_get_destination());
    */
  }


  /**
  * Prepares the taxonomy vocabulary for saving an org tree.
  */
  public static function prepareVocabulary() {
    $vocab = Orgs::getVocabulary();

    if (!$vocab) {

      /* Do we want to create a vocabulary here? It should be created when
       * the module is installed. This would be the case where the user
       * accidentally deleted the vocabulary...
       * Anyway, here's a start:
       * http://www.drupal8.ovh/en/tutoriels/68/create-taxonomy-vocabulary-programmatically-on-drupal-8

      $vocab = new \StdClass();
      $vocab->name = t('CAPx Organizations');
      $vocab->machine_name = Orgs::getVocabularyMachineName();
      $vocab->description = t("A hierarchy of organization codes and information");
      $vocab->module = "stanford_capx";
      taxonomy_vocabulary_save($vocab);
      */
    }

    if (!$vocab->vid) {
      throw new \Exception("Could not find CAPx Organization Taxonomy Vocabulary");
    }
  }


  /**
   * Returns the vocabulary object that orgs use.
   *
   * @return object
   *   The vocabulary object that is being used.
   */
  public static function getVocabulary() {
    return Vocabulary::load(Orgs::getVocabularyMachineName());
  }

  /**
   * Gets vocabulary machine name.
   *
   * Returns the vocabulary machine name that we decided to use for storing
   * the organization terms.
   *
   * @return string
   *   Machine name of the vocabulary for storing organization codes.
   */
  public static function getVocabularyMachineName() {
    return Orgs::$vocabularyMachineName;
  }

}