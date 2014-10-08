<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\APILib\HTTPClient;
use CAPx\Drupal\Entities\CFEntity;

interface ImporterInterface {

  /**
   * [__construct description]
   * @param array $config [description]
   */
  public function __construct(CFEntity $importer, EntityMapper $mapper, HTTPClient $client);

}
