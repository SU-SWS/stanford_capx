<?php
/**
 * @file
 * CAPx Tamper classes to fake feeds importer.
 */

use \CAPx\Drupal\Entities\CFEntity;

/**
 * Class CapxTamper.
 */
class CapxTamper {

  /**
   * Mapper to use for processing.
   *
   * @var CFEntity
   */
  public $mapper;
  public $parser;
  public $processor;

  /**
   * CapxTamper constructor.
   *
   * @param \CAPx\Drupal\Entities\CFEntity $mapper
   *   Mapper in use.
   */
  public function __construct(CFEntity $mapper) {
    $this->mapper = $mapper;
    $this->parser = new CapxTamperParser();
    $this->processor = new CapxTamperProcessor($mapper);
  }

}

/**
 * Class CapxTamperParser.
 */
class CapxTamperParser {

  /**
   * Simulates FeedsCSVParser::getMappingSources().
   *
   * @return bool
   *   Similar to FeedsCSVParser
   */
  public function getMappingSources() {
    return FALSE;
  }

}

/**
 * Class CapxTamperProcessor.
 */
class CapxTamperProcessor {

  /**
   * Mapper to use for processing.
   *
   * @var CFEntity
   */
  public $mapper;

  /**
   * CapxTamperProcessor constructor.
   *
   * @param CFEntity $mapper
   *   CAPx Mapper to use.
   */
  public function __construct(CFEntity $mapper) {
    $this->mapper = $mapper;
  }

  /**
   * Passes the available sources & targets.
   *
   * @return array
   *   Available sources.
   */
  public function getMappings() {
    $mapper_sources = capx_tamper_get_mapper_sources($this->mapper);
    $sources = array();
    foreach ($mapper_sources as $path => $targets) {
      $sources[] = array(
        'source' => $path,
        'target' => implode(':', $targets),
        'unique' => FALSE,
        'language' => LANGUAGE_NONE,
      );
    }
    return $sources;
  }

}