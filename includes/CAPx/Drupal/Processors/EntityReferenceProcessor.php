<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;

use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Util\CAPx;

class EntityReferenceProcessor {

  protected $entity;
  protected $importer;
  protected $target;

  /**
   * Creates an entityReferenceProcessor to handle entity reference fields.
   * @param [type] $entity   [description]
   * @param [type] $importer [description]
   * @param [type] $target   [description]
   */
  public function __construct($entity, $importer, $target) {
    $this->entity = $entity;
    $this->importer = $importer;
    $this->fieldName = $fieldName;
    $this->target = $target;
  }

  /**
   * Returns a list of possible matches.
   * @return [type] [description]
   */
  public function execute() {

    // Get the profile ID of this entity as the profile id will be the same
    // for other importers and entity/bundle types.

    $profile_id = $this->entity->value()->capx['profileId'];

    // Did not find one. It could be that is hasn't been created yet and may
    // take another cycle or two to come up.
    if (!$profile_id) {
      throw new \Exception('Could not find profileId. Did something change in the API?');
    }

    $match = db_select("capx_profiles", 'capx')
      ->fields('capx')
      ->condition('profile_id', $profile_id)
      ->condition('importer', $this->target)
      ->orderBy('id', 'DESC')
      ->execute()
      ->fetchAssoc();

    if (empty($match)) {
      return array();
    }

    // try to load it.
    $entity = entity_load_single($match['entity_type'], $match['entity_id']);

    // Return the result.
    return empty($entity) ? array() : $entity;
  }


}
