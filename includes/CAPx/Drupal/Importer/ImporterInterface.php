<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;

use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\APILib\HTTPClient;

/**
 * The interface for entity importers.
 */
interface ImporterInterface {

  /**
   * A __construct description.
   *
   * @param object $importer
   *   [description]
   * @param object $mapper
   *   [description]
   * @param object $client
   *   [description]
   */
  public function __construct(CFEntity $importer, EntityMapper $mapper, HTTPClient $client);

}
