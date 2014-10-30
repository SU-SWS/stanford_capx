<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Util;

use CAPx\APILib\HTTPClient;

use CAPx\Drupal\Util\CAPx;
use CAPx\Drupal\Util\CAPxMapper;
use CAPx\Drupal\Util\CAPxConnection;
use CAPx\Drupal\Importer\EntityImporter;

use CAPx\Drupal\Importer\Orphans\EntityImporterOrphans;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareMissingFromApi;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareOrgCodesSunet;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareOrgCodesWorkgroups;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareSunetOrgCodes;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareSunetWorkgroups;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareWorkgroupsOrgCodes;
use CAPx\Drupal\Importer\Orphans\Comparisons\CompareWorkgroupsSunet;

use CAPx\Drupal\Importer\Orphans\Lookups\LookupMissingFromAPI;
use CAPx\Drupal\Importer\Orphans\Lookups\LookupMissingFromSunetList;
use CAPx\Drupal\Importer\Orphans\Lookups\LookupOrgOrphans;
use CAPx\Drupal\Importer\Orphans\Lookups\LookupWorkgroupOrphans;

class CAPxImporter {

  /**
   * Wrapper for capx_cfe_load_multiple(mappers).
   *
   * @return array
   *   An array of loaded importers
   */
  public static function loadAllImporters() {
    return capx_cfe_load_multiple(FALSE, array('type' => 'importer'));
  }

  /**
   * Wrapper for capx_cfe_load_by_machine_name & capx_cfe_load.
   *
   * Loads the configuration entity and not the entity importer class which
   * does the actual importing.
   *
   * @param mixed $key
   *   int - cfid
   *   string - machine name
   *
   * @return array
   *  One loaded importer.
   */
  public static function loadImporter($key) {

    if (is_numeric($key)) {
      return capx_cfe_load_multiple($key, array('type' => 'importer'));
    }
    else {
      return capx_cfe_load_by_machine_name($key, 'importer');
    }

  }

  /**
   * Loads an EntityImporter by machine name or id.
   *
   * @param mixed $key
   *   Either a machine name or id.
   *
   * @return EntityImporter
   *   A fully instantiated EntityImporter
   */
  public static function loadEntityImporter($key) {
    $entityImporter = NULl;

    $importerConfig = self::loadImporter($key);
    $mapper = CAPxMapper::loadEntityMapper($importerConfig->mapper);
    $client = CAPxConnection::getAuthenticatedHTTPClient();

    if ($importerConfig && $mapper && $client) {
      $entityImporter = new EntityImporter($importerConfig, $mapper, $client);
    }

    return $entityImporter;
  }

  /**
   * Loads EntityImporter's filtered by mapper.
   *
   * @param CFEntity $mapper
   *   The configuration entity mapper
   *
   * @return array
   *   An arry of loaded importers that use the passed in mapper.
   */
  public static function loadImportersByMapper($mapper) {
    $importers = self::loadAllImporters();

    foreach ($importers as $id => $importer) {
      if ($importer->mapper != $mapper->identifier()) {
        unset($importers[$id]);
      }
    }

    return $importers;
  }

  /**
   * Return the options for running cron on an importer.
   *
   * @return array
   *   An array of options for a select field.
   */
  public static function getCronOptions() {
    return array(
      'none' => t('Do not sync'),
      'daily' => t('Every day'),
      'all' => t('As often as possible'),
    );
  }

  /**
   * Return a fully loaded EntityImporterOrphans.
   *
   * @param  [type] $profiles [description]
   * @param  [type] $importer [description]
   * @return [type]           [description]
   */
  public static function getEntityOrphanator($importerName, $profiles = array()) {
    $orphanator = NULL;

    $importer = CAPxImporter::loadEntityImporter($importerName);

    if ($importer) {
      $lookups = array();
      $comparisons = array();

      // Load all the lookups...
      $lookups[] = new LookupMissingFromAPI();
      $lookups[] = new LookupMissingFromSunetList();
      $lookups[] = new LookupOrgOrphans();
      $lookups[] = new LookupWorkgroupOrphans();

      // Load all the comparisons...
      $comparisons[] = new CompareMissingFromApi();
      $comparisons[] = new CompareOrgCodesSunet();
      $comparisons[] = new CompareOrgCodesWorkgroups();
      $comparisons[] = new CompareSunetOrgCodes();
      $comparisons[] = new CompareSunetWorkgroups();
      $comparisons[] = new CompareWorkgroupsOrgCodes();
      $comparisons[] = new CompareWorkgroupsSunet();

      $orphanator = new EntityImporterOrphans($importer, $profiles, $lookups, $comparisons);
    }
    else {
      $vars = array(
        '%name' => $importerName,
        '!log' => l(t('log messages'), 'admin/reports/dblog'),
      );
      drupal_set_message(t('There was an issue loading the importer with %name machine name. Check !log.', $vars), 'error');
    }

    return $orphanator;
  }
}
