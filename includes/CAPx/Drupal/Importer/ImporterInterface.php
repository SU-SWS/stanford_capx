<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\APILib\HTTPClient;

interface ImporterInterface {

  /**
   * [__construct description]
   * @param array $config [description]
   */
  public function __construct(Array $config, EntityMapper $mapper, HTTPClient $client);

}
