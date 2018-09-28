<?php
/**
 * Created by PhpStorm.
 * User: cjwest
 * Date: 9/27/18
 * Time: 20:46
 */

namespace Drupal\stanford_capx\Includes\Organizations;


class Orgs {
  public static $vocabularyMachineName = "capx_organizations";

  /**
  * Prepares the taxonomy vocabulary for saving an org tree.
  */
  public static function prepareVocabulary() {
    $vocab = Orgs::getVocabulary();

    if (!$vocab) {
      $vocab = new \StdClass();
      $vocab->name = t('CAPx Organizations');
      $vocab->machine_name = Orgs::getVocabularyMachineName();
      $vocab->description = t("A hierarchy of organization codes and information");
      $vocab->module = "stanford_capx";
      taxonomy_vocabulary_save($vocab);
    }

    if (!$vocab->vid) {
      throw new \Exception("Could not create or find CAPx Organization Taxonomy Vocabulary");
    }
  }


  /**
   * Returns the vocabulary object that orgs use.
   *
   * @return object
   *   The vocabulary object that is being used.
   */
  public static function getVocabulary() {
    return taxonomy_vocabulary_machine_name_load(Orgs::getVocabularyMachineName());
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