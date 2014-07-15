<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;
use CAPx\Drupal\Mapper\EntityMapper;

interface ProcessorInterface {

  /**
   * [__construct description]
   * @param $mapper  [description]
   * @param Array  $capData [description]
   */
  public function __construct($mapper, $capData);

  /**
   * [execute description]
   * @return [type] [description]
   */
  public function execute();

}
