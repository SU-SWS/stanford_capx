<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;

use CAPx\Drupal\Mapper\EntityMapper as EntityMapper;
use CAPx\APILib\HTTPClient as HTTPClient;

/**
 * The interface for entity importers.
 */
interface ImporterInterface {

  /**
   * A __construct description.
   *
   * @param \CFEntity $importer
   *   [description]
   * @param EntityMapper $mapper
   *   [description]
   * @param HTTPClient $client
   *   [description]
   */
  public function __construct(\CFEntity $importer, EntityMapper $mapper, HTTPClient $client);

}
