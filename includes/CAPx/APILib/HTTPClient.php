<?php
/**
 * CAP HTTPClient powered by Guzzle :)
 */

namespace CAPx\APILib;
use \Guzzle\Http\Client as GuzzleClient;

class HTTPClient {

  protected $http_client = null;
  // Endpoint
  protected $http_endpoint = 'https://cap.stanford.edu/cap-api';
  // Auth Token
  protected $http_auth_token;
  // HTTP Options
  protected $http_options;

  /**
   * [__construct description]
   */
  public function __construct() {
    $client = new GuzzleClient();
    $this->set_http_client($client);
  }

  /**
   * [get_http_endpoint description]
   * @return [type] [description]
   */
  public function get_http_endpoint() {
    return $this->http_endpoint;
  }

  /**
   * [set_http_endpoint description]
   * @param [type] $end [description]
   */
  public function set_http_endpoint($end) {

    // When the endpoint changes create a new client.
    $client = new GuzzleClient($end);
    $this->set_http_client($client);

    $this->http_endpoint = $end;
  }

  /**
   * [get_http_client description]
   * @return [type] [description]
   */
  public function get_http_client() {

    if (!is_null($this->http_client)) {
      return $this->http_client;
    }

    $client = new GuzzleClient($this->get_http_endpoint());
    $this->set_http_client($client);
    return $client;
  }

  /**
   * [set_http_client description]
   * @param [type] $client [description]
   */
  public function set_http_client($client) {
    $this->http_client = $client;
  }

  /**
   * [set_api_token description]
   * @param [type] $token [description]
   */
  public function set_api_token($token) {
    $this->http_auth_token = $token;
  }

  /**
   * [get_api_token description]
   * @return [type] [description]
   */
  protected function get_api_token() {
    if (empty($this->http_auth_token)) {
      // Try to authenticate
    }
    return $this->http_auth_token;
  }

  /**
   * [get_http_options description]
   * @return [type] [description]
   */
  public function get_http_options() {
    return $this->http_options;
  }

  /**
   * [set_http_options description]
   * @param [type] $opts [description]
   */
  public function set_http_options($opts) {
    $this->http_options = $opts;
  }

  //
  // ---------------------------------------------------------------------------
  //

  /**
   *
   * @param  [type] $name [description]
   * @return [type]       [description]
   */
  public function api($name) {

    $client = $this->get_http_client();
    $options = $this->get_http_options();

    // Add access token or we wont be able to communicate.
    $options['query']['access_token'] = $this->get_api_token();

    switch ($name) {
      case "auth":
        $api = new \CAPx\APILib\AuthLib\AuthLib($client);
        break;
      case "org":
        $api = new \CAPx\APILib\OrgLib\OrgLib($client, $options);
        break;
      case "profile":
        $api = new \CAPx\APILib\ProfileLib\ProfileLib($client, $options);
        break;
      case "schema":
        $api = new \CAPx\APILib\SchemaLib\SchemaLib($client, $options);
        break;
      case "search":
        $api = new \CAPx\APILib\SearchLib\SearchLib($client, $options);
        break;
      case "layouts":
        $api = new \CAPx\APILib\LayoutsLib\LayoutsLib($client, $options);
        break;
      default:
        throw new Exception(sprintf('Undefined api instance called: "%s"', $name));
    }

  return $api;
  }

}
