<?php
/**
 * @file
 *
 * A file to tie into Drupal module and core hooks.
 *
 * @author [author] <[email]>
 */

use CAPx\Drupal\Util\CAPx;

/**
 * implements hook_entity_delete
 * @param  [type] $entity [description]
 * @param  [type] $type   [description]
 * @return [type]         [description]
 */
function stanford_capx_entity_delete($entity, $type) {

  $entity = entity_metadata_wrapper($type, $entity);
  CAPx::deleteProfileRecord($entity);

}

/**
 * [stanford_capx_entity_insert description]
 * @param  [type] $entity [description]
 * @param  [type] $type   [description]
 * @return [type]         [description]
 */
function stanford_capx_entity_insert($entity, $type) {

}


/**
 * [stanford_capx_entity_update description]
 * @param  [type] $entity [description]
 * @param  [type] $type   [description]
 * @return [type]         [description]
 */
function stanford_capx_entity_update($entity, $type) {

}

/**
 * [stanford_capx_entity_load description]
 * @param  [type] $entities [description]
 * @param  [type] $type     [description]
 * @return [type]           [description]
 */
function stanford_capx_entity_load($entities, $type) {

}

/**
 * [stanford_capx_entity_view description]
 * @param  [type] $entity    [description]
 * @param  [type] $type      [description]
 * @param  [type] $view_mode [description]
 * @param  [type] $langcode  [description]
 * @return [type]            [description]
 */
function stanford_capx_entity_view($entity, $type, $view_mode, $langcode) {

}
/**
 * Wrapper for entity_get_info() with restricted types removed.
 * @return [type] [description]
 */
function capx_entity_get_info() {
  global $CAP_RESTRICTED_ENTITY_TYPES;
  $entity_types = entity_get_info();
  foreach ($entity_types as $entity_name => $values) {
    if (in_array($entity_name, $CAP_RESTRICTED_ENTITY_TYPES)) {
      unset($entity_types[$entity_name]);
    }
  }
  return $entity_types;
}
