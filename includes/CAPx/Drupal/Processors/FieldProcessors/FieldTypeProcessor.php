<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;
use CAPx\Drupal\Processors\FieldProcessors\FieldProcessorAbstract;


class FieldTypeProcessor extends FieldProcessorAbstract {

  /**
   * [__construct description]
   * @param [type] $entity    [description]
   * @param [type] $fieldName [description]
   * @param [type] $type      [description]
   */
  public function __construct($entity, $fieldName, $type = null) {
    parent::__construct($entity, $fieldName, $type);
  }


  /**
   * Default implementation of widget function.
   * @param  [type] $type      [description]
   * @return [type]            [description]
   */
  public function widget($type) {
    return $this;
  }

}
