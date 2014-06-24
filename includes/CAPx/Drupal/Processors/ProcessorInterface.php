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
   * @param EntityMapper $mapper  [description]
   * @param Array  $capData [description]
   */
  public function __construct(EntityMapper $mapper, Array $capData);

  /**
   * [execute description]
   * @return [type] [description]
   */
  public function execute();

}
