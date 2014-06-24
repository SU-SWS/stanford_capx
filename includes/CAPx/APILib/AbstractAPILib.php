<?php
/**
 * @file
 * API Library
 */

namespace CAPx\APILib;
use CAPx\APILib\AbstractAPILibInterface;
use \Guzzle\Http\Client as GuzzleClient;

abstract class AbstractAPILib implements AbstractAPILibInterface {

  // CAPx\HTTPClient
  protected $client;
  // Endpoint
  protected $endpoint = 'https://cap.stanford.edu/cap-api';
  // Request Options
  protected $options = array();

  /**
   * [__construct description]
   */
  public function __construct(GuzzleClient $client, $options = null) {

    $this->setClient($client);

    // Merge in any additional options.
    if (is_array($options)) {
      $opts = $this->getOptions();
      $opts = array_merge($opts, $options);
      $this->setOptions($opts);
    }

  }

  /**
   * [setClient description]
   * @param [type] $client [description]
   */
  public function setClient($client) {
    $this->client = $client;
  }

  /**
   * [getClient description]
   * @return [type] [description]
   */
  public function getClient() {
    return $this->client;
  }

  /**
   * [setEndpoint description]
   * @param  [type] $endpoint [description]
   * @return [type]           [description]
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }

  /**
   * [getEndpoint description]
   * @return [type] [description]
   */
  public function getEndpoint() {
    return $this->endpoint;
  }

  /**
   * [setOptions description]
   * @param  [type] $options [description]
   * @return [type]           [description]
   */
  public function setOptions($options) {
    $this->options = $options;
  }

  /**
   * [getOptions description]
   * @return [type] [description]
   */
  public function getOptions() {
    return $this->options;
  }

  /**
   * [makeRequest description]
   * @param  [type] $endpoint [description]
   * @param  [type] $params   [description]
   * @param  [type] $options  [description]
   * @return [type]           [description]
   */
  protected function makeRequest($endpoint, $params, $options) {
    $client = $this->getClient();
    $request = $client->get($endpoint, $params, $options);
    $response = $request->send();

    $code = $response->getStatusCode();
    switch ($code) {
      case '200':
        $json = $response->json();
        return $json;
        break;

      default:
        return FALSE;
        break;
    }

  }


}
