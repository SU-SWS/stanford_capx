<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Importer;
use CAPx\Drupal\Util\CAPx;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Processors\EntityProcessor;
use CAPx\APILib\HTTPClient;


class EntityImporter implements ImporterInterface {

  // Options and configuration array
  protected $options = array();

  // The mapping scheme object
  protected $mapper;

  /**
   * [__construct description]
   * @param [type] $config [description]
   */
  public function __construct(Array $config, EntityMapper $mapper, HTTPClient $client) {
    $this->addOptions($config);
    $this->setMapper($mapper);
    $this->setClient($client);
  }

  /**
   * Oh man. Oh man. Oh man. Do all the things.
   * @return [type] [description]
   */
  public function execute() {

    $options = $this->getOptions();
    $mapper = $this->getMapper();
    $client = $this->getClient();

    $data = $client->api('profile')->search($options['type'], $options['values']);
    CAPx::setData($data);

    if(isset($data['values'])) {
      foreach ($data['values'] as $index => $info) {
        $processor = new EntityProcessor($mapper, $info);
        $processor->execute();
      }
    }

  }

  // ===========================================================================
  // GETTERS & SETTERS
  // ===========================================================================

  /**
   * [getOptions description]
   * @return [type] [description]
   */
  protected function getOptions() {
    return $this->options;
  }

  /**
   * [addOptions description]
   * @param [type] $newOpts [description]
   */
  public function addOptions($newOpts) {
    $opts = $this->getOptions();
    $opts = array_merge($opts, $newOpts);
    $this->setOptions($opts);
  }

  /**
   * [setOptions description]
   * @param [type] $opts [description]
   */
  protected function setOptions($opts) {
    $this->options = $opts;
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
  public function setMapper($map) {
    $this->mapper = $map;
  }

  /**
   * [getClient description]
   * @return [type] [description]
   */
  public function getClient() {
    return $this->client;
  }

  /**
   * [setClient description]
   * @param [type] $client [description]
   */
  public function setClient($client) {
    $this->client = $client;
  }



}
