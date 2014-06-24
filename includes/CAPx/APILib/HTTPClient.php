<?php
/**
 * CAP HTTPClient powered by Guzzle :)
 */

namespace CAPx\APILib;
use \Guzzle\Http\Client as GuzzleClient;

class HTTPClient {

  protected $httpClient = null;
  // Endpoint
  protected $httpEndpoint = 'https://cap.stanford.edu/cap-api';
  // Auth Token
  protected $httpAuthToken;
  // HTTP Options
  protected $httpOptions;

  /**
   * [__construct description]
   */
  public function __construct() {
    $client = new GuzzleClient();
    $this->setHttpClient($client);
  }

  /**
   * [getHttpEndpoint description]
   * @return [type] [description]
   */
  public function getHttpEndpoint() {
    return $this->httpEndpoint;
  }

  /**
   * [setHttpEndpoint description]
   * @param [type] $end [description]
   */
  public function setHttpEndpoint($end) {

    // When the endpoint changes create a new client.
    $client = new GuzzleClient($end);
    $this->setHttpClient($client);

    $this->httpEndpoint = $end;
  }

  /**
   * [getHttpClient description]
   * @return [type] [description]
   */
  public function getHttpClient() {

    if (!is_null($this->httpClient)) {
      return $this->httpClient;
    }

    $client = new GuzzleClient($this->getHttpEndpoint());
    $this->setHttpClient($client);
    return $client;
  }

  /**
   * [setHttpClient description]
   * @param [type] $client [description]
   */
  public function setHttpClient($client) {
    $this->httpClient = $client;
  }

  /**
   * [setApiToken description]
   * @param [type] $token [description]
   */
  public function setApiToken($token) {
    $this->httpAuthToken = $token;
  }

  /**
   * [getApiToken description]
   * @return [type] [description]
   */
  protected function getApiToken() {
    if (empty($this->httpAuthToken)) {
      // Try to authenticate
    }
    return $this->httpAuthToken;
  }

  /**
   * [gethttpOptions description]
   * @return [type] [description]
   */
  public function gethttpOptions() {
    return $this->httpOptions;
  }

  /**
   * [sethttpOptions description]
   * @param [type] $opts [description]
   */
  public function sethttpOptions($opts) {
    $this->httpOptions = $opts;
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

    $client = $this->getHttpClient();
    $options = $this->gethttpOptions();

    // Add access token or we wont be able to communicate.
    $options['query']['access_token'] = $this->getApiToken();

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
