<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Util;
use CAPx\APILib\HTTPClient;
use Guzzle\Http\Exception\ClientErrorResponseException;

class CAPx {

  /**
   * Returns an array of loaded profile entities.
   *
   * @param string $type
   *   The entity type to load
   * @param array $conditions
   *   An array of conditions to use in the DB query when looking for profiles.
   *
   * @return array
   *   An array of loaded profile entities.
   */
  public static function getProfiles($type, $conditions = array()) {

    if (!$type) {
      throw new \Exception("Type required for getProfiles", 1);
    }

    $query = db_select("capx_profiles", 'capx')
      ->fields('capx', array('entity_type', 'entity_id'))
      ->condition('entity_type', $type)
      ->orderBy('id', 'DESC');

    foreach ($conditions as $key => $value) {
      $query->condition($key, $value);
    }

    $result = $query->execute();
    $assoc = $result->fetchAllAssoc('entity_id');
    $ids = array_keys($assoc);

    return entity_load($type, $ids);
  }

  /**
   * Returns a fully loaded entity from the DB.
   *
   * @param string $entityType
   *   The entity type (eg: node, block, user)
   * @param string $bundleType
   *   The name of the bundle we a loading (eg: page, article)
   * @param int $profileId
   *   The profile id from the CAP API.
   *
   * @return object
   *   A fully loaded entity from the DB.
   */
  public static function getEntityByProfileId($entityType, $bundleType, $profileId) {
    // @todo: CACHE THIS!

    $entityId = CAPx::getEntityIdByProfileId($entityType, $bundleType, $profileId);

    if (!$entityId) {
      return FALSE;
    }

    $entity = entity_load_single($entityType, $entityId);
    return $entity;

  }

  /**
   * Returns an entity id by its profile id, type, and bundle.
   *
   * @param string $entityType
   *   The entity type (eg: node, block, user)
   * @param string $bundleType
   *   The name of the bundle we a loading (eg: page, article)
   * @param int $profileId
   *   The profile id from the CAP API.
   *
   * @return int
   *   The entity id (not CAP API id)
   */
  public static function getEntityIdByProfileId($entityType, $bundleType, $profileId) {

    $query = db_select("capx_profiles", 'capx')
      ->fields('capx', array('entity_id'))
      ->condition('entity_type', $entityType)
      ->condition('bundle_type', $bundleType)
      ->condition('profile_id', $profileId)
      ->orderBy('id', 'DESC')
      ->execute()
      ->fetchAssoc();

    return isset($query['entity_id']) ? $query['entity_id'] : FALSE;

  }

  /**
   * Returns the profile Id of a loaded entity.
   *
   * @param object $entity
   *   A loaded entity object
   *
   * @return int
   *   The cap API profile id.
   */
  public static function getProfileIdByEntity($entity) {
    $id = $entity->getIdentifier();
    $entityType = $entity->type();
    $bundleType = $entity->getBundle();

    $query = db_select("capx_profiles", 'capx')
      ->fields('capx', array('profile_id'))
      ->condition('entity_type', $entityType)
      ->condition('bundle_type', $bundleType)
      ->condition('entity_id', $id)
      ->orderBy('id', 'DESC')
      ->execute()
      ->fetchAssoc();

    return isset($query['profile_id']) ? $query['profile_id'] : FALSE;
  }

  /**
   * Create new profile record.
   *
   * Inserts a record into the capx_profiles table with information that helps
   * the rest of the module keep track of what it is and where it came from.
   *
   * @param Entity $entity
   *   The entity that was just saved.
   */
  public static function insertNewProfileRecord($entity, $profileId, $etag, $importer) {
    $id = $entity->getIdentifier();
    $entityType = $entity->type();
    $bundleType = $entity->getBundle();

    $record = array(
      'entity_type' => $entityType,
      'entity_id' => $id,
      'importer' => $importer,
      'profile_id' => $profileId,
      'etag' => $etag,
      'bundle_type' => $bundleType,
      'sync' => 1,
    );

    $yes = drupal_write_record('capx_profiles', $record);

    if (!$yes) {
      watchdog('CAPx', 'Could not insert record for capx_profiles on profile id: ' . $profileId, array(), WATCHDOG_ERROR);
    }
  }

  /**
   * Get the etag for an entity.
   * @param  [type] $importer  [description]
   * @param  [type] $profileId [description]
   * @return [type]            [description]
   */
  public static function getEntityETag($importer, $profileId) {

    $result = db_select('capx_profiles', 'capxp')
      ->fields('capxp', array('etag'))
      ->condition('importer', $importer)
      ->condition("profile_id", $profileId)
      ->execute();

    $etag = $result->fetchField();
    if (!is_numeric($etag)) {
      return FALSE;
    }

    return $etag;
  }

  /**
   * Remove a profile record.
   *
   * Removes a profile record from the capx_profiles table when an entity is
   * deleted. No longer need to keep track of it.
   *
   * @param Entity $entity
   *   The entity that is being deleted.
   */
  public static function deleteProfileRecord($entity) {

    // BEAN is returning its delta when using this.
    // $id = $entity->getIdentifier();

    $entityType = $entity->type();
    $entityRaw = $entity->raw();
    list($id, $vid, $bundle) = entity_extract_ids($entityType, $entityRaw);

    db_delete('capx_profiles')
      ->condition('entity_type', $entityType)
      ->condition('entity_id', $id)
      ->execute();
  }

  /**
   * Returns the API endpoint.
   * @todo Move this to CAPxConnection
   *
   * @return string
   *  Full URL to the API endpoint
   */
  public static function getAPIEndpoint() {
    return variable_get('stanford_capx_api_base_url', 'https://api.stanford.edu');
  }

  /**
   * Returns the authentication endpoint.
   *
   * @todo Move this to CAPxConnection
   * @return string
   *   Full url to the auth endpoint
   */
  public static function getAuthEndpoint() {
    return variable_get('stanford_capx_api_auth_uri', 'https://authz.stanford.edu/oauth/token');
  }

  /**
   * Returns a decrypted username that authenticates with the cap api.
   *
   * @todo Mmove this to CAPxConnection
   *
   * @return string
   *   The username
   */
  public static function getAuthUsername() {
    return decrypt(variable_get('stanford_capx_username', ''));
  }

  /**
   * Returns a decrypted password that authenticates with the cap api.
   * @todo Move this to CAPxConnection
   * @return string
   *   The password
   */
  public static function getAuthPassword() {
    return decrypt(variable_get('stanford_capx_password', ''));
  }


}
