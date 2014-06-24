<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;
use CAPx\Drupal\Mapper\EntityMapper;

abstract class ProcessorAbstract implements ProcessorInterface {

  protected $mapper;
  protected $data;

  /**
   * [__construct description]
   * @param EntityMapper $mapper  [description]
   * @param Array  $capData [description]
   */
  public function __construct(EntityMapper $mapper, Array $data) {
    $this->setMapper($mapper);
    $this->setData($data);
  }

  // ===========================================================================
  // GETTERS & SETTERS
  // ===========================================================================

  /**
   * [getData description]
   * @return [type] [description]
   */
  protected function getData() {
    return $this->data;
  }

  /**
   * [setData description]
   * @param [type] $opts [description]
   */
  protected function setData(Array $data) {
    $this->data = $data;
  }

  /**
   * [getMapper description]
   * @return [type] [description]
   */
  public function getMapper() {
    return $this->mapper;
  }

  /**
   * [setMapper description]
   * @param [type] $map [description]
   */
  public function setMapper(EntityMapper $map) {
    $this->mapper = $map;
  }

}
