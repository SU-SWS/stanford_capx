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

    $this->set_client($client);

    // Merge in any additional options.
    if (is_array($options)) {
      $opts = $this->get_options();
      $opts = array_merge($opts, $options);
      $this->set_options($opts);
    }

  }

  /**
   * [set_client description]
   * @param [type] $client [description]
   */
  public function set_client($client) {
    $this->client = $client;
  }

  /**
   * [get_client description]
   * @return [type] [description]
   */
  public function get_client() {
    return $this->client;
  }

  /**
   * [set_endpoint description]
   * @param  [type] $endpoint [description]
   * @return [type]           [description]
   */
  public function set_endpoint($endpoint) {
    $this->endpoint = $endpoint;
  }

  /**
   * [get_endpoint description]
   * @return [type] [description]
   */
  public function get_endpoint() {
    return $this->endpoint;
  }

  /**
   * [set_options description]
   * @param  [type] $options [description]
   * @return [type]           [description]
   */
  public function set_options($options) {
    $this->options = $options;
  }

  /**
   * [get_options description]
   * @return [type] [description]
   */
  public function get_options() {
    return $this->options;
  }

  /**
   * [make_request description]
   * @param  [type] $endpoint [description]
   * @param  [type] $params   [description]
   * @param  [type] $options  [description]
   * @return [type]           [description]
   */
  protected function make_request($endpoint, $params, $options) {
    $client = $this->get_client();
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
