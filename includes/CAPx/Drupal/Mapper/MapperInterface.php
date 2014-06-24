<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

interface MapperInterface {

  /**
   * [execute description]
   * @param  [type] $entity [description]
   * @param  [type] $data   [description]
   * @return [type]         [description]
   */
  public function execute($entity, $data);

}
