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
   * @param  array  $conditions [description]
   * @return [type]             [description]
   */
  public static function getProfiles($type, $conditions = array()) {

    if (!$type) {
      throw new Exception("Type required for getProfiles", 1);
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
   * Returns a fully loaded entity from the DB
   * @param  [type] $profileId [description]
   * @return [type]            [description]
   */
  public static function getEntityByProfileId($entityType, $bundleType, $profileId) {
    // @TODO: CACHE THIS!

    $entityId = CAPx::getEntityIdByProfileId($entityType, $bundleType, $profileId);

    if (!$entityId) {
      return FALSE;
    }

    $entity =  entity_load_single($entityType, $entityId);
    return $entity;

  }

  /**
   * Returns an entity by its profile id, type, and bundle.
   * @param  [type] $entityType [description]
   * @param  [type] $bundleType [description]
   * @param  [type] $profileId  [description]
   * @return [type]             [description]
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
   * @param  [type] $entity [description]
   * @return [type]         [description]
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
   * Inserts a record into the capx_profiles table with information that helps
   * the rest of the module keep track of what it is and where it came from.
   * @param  Entity $entity the entity that was just saved.
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
   * Removes a profile record from the capx_profiles table when an entity is
   * deleted. No longer need to keep track of it.
   * @param  Entity $entity the entity that is being deleted.
   */
  public static function deleteProfileRecord($entity) {

    // BEAN is returning its delta when using this.
    //$id = $entity->getIdentifier();

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
   * @return string full URL to the API endpoint
   */
  public static function getAPIEndpoint() {
    return variable_get('stanford_capx_api_base_url', 'https://api.stanford.edu');
  }

  /**
   * Returns the authentication endpoint
   * @return string full url to the auth endpoint
   */
  public static function getAuthEndpoint() {
    return variable_get('stanford_capx_api_auth_uri', 'https://authz.stanford.edu/oauth/token');
  }

  /**
   * Returns a decrypted username that authenticates with the cap api.
   * @return string the username
   */
  public static function getAuthUsername() {
    return decrypt(variable_get('stanford_capx_username', ''));
  }

  /**
   * Returns a decrypted password that authenticates with the cap api.
   * @return string the password
   */
  public static function getAuthPassword() {
    return decrypt(variable_get('stanford_capx_password', ''));
  }

}
