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
