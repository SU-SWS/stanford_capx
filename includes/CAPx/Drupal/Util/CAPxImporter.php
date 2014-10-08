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

class CAPxImporter {

  /**
   * Wrapper for capx_cfe_load_multiple(mappers)
   * @return [type] [description]
   */
  public static function loadAllImporters() {
    return capx_cfe_load_multiple(FALSE, array('type' => 'importer'));
  }

  /**
   * Wrapper for capx_cfe_load_by_machine_name & capx_cfe_load
   * @param  [type] $key [description]
   * @return [type]      [description]
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
   * @param  mixed $key either a machine name or id.
   * @return EntityImporter      a fully instantiated EntityImporter
   */
  public static function loadEntityImporter($key) {

    $importer = self::loadImporter($key);
    $mapper = CAPxMapper::loadEntityMapper($importer->mapper);
    $client = CAPxConnection::getAuthenticatedHTTPClient();

    $entityImporter = new EntityImporter($importer, $mapper, $client);
    return $entityImporter;
  }

  /**
   * Loads EntityImporter's filtered by mapper.
   * @param  [type] $mapper [description]
   * @return [type] [description]
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
   * @return array An array of options for a select field.
   */
  public static function getCronOptions() {
    return array(
      'none' => t('Do not sync'),
      'daily' => t('Every day'),
      'all' => t('As often as possible'),
    );
  }
}
