<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class EntityReferenceFieldProcessor extends FieldTypeProcessor {

  /**
   * Default implementation of put.
   *
   * @see FieldProcessorAbstract::put()
   */
  public function put($data) {
    $data = $this->findProfileID($data);
    parent::put($data);
  }

  /**
   * Prepares CAP API data to feet to Drupal field.
   *
   * @param array $data
   *   CAP API field data.
   *
   * @return array
   *   Prepared data.
   */
  public function findProfileID($data) {
    return 82;
  }

}
