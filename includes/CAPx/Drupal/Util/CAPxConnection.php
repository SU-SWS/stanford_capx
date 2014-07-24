<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Util;
use CAPx\APILib\HTTPClient;
use Guzzle\Http\Exception\ClientErrorResponseException;

class CAPxConnection {
   /**
   * Test that both the API and Auth endpoints work.
   * @return [type] [description]
   */
  public static function testConnection() {
    $auth = CAPxConnection::testAuthConnection();
    $api  = CAPxConnection::testApiConnection();

    if ($auth->status && $api->status) {
      return $auth;
    }

    if (!$auth->status) {
      return $auth;
    }

    return $api;
  }

  /**
   * [testConnection description]
   * @return [type] [description]
   */
  public static function testAuthConnection($username = null, $password = null, $authpoint = null) {

    $return = (object) array(
      'status' => 0,
      'message' => t('connection failed'),
      'code' => 0,
    );

    $username = is_null($username)    ? CAPx::getAuthUsername() : $username;
    $password = is_null($password)    ? CAPx::getAuthPassword() : $password;
    $authpoint = is_null($authpoint)  ? CAPx::getAuthEndpoint() : $authpoint;

    $client = new HTTPClient();
    $client->setEndpoint($authpoint);

    try {
      $auth = $client->api('auth')->authenticate($username, $password);
    }
    catch(\Exception $e) {
      $return->message = t($e->getMessage());
      return $return;
    }

    $token = $auth->getAuthApiToken();
    $response = $auth->getLastResponse();
    $reasonPhrase = $response->getReasonPhrase();
    $code = $response->getStatusCode();

    $return->code = $code;
    $return->message = t($reasonPhrase);

    if (!empty($token)) {
      $return->status = 1;
      $return->token = $token;
    }

    return $return;
  }


  /**
   * Tests a token against the API for validity
   * @param  [type] $token [description]
   * @return object        $object->value
   *                       $obj
   */
  public static function testApiConnection($token = null, $endpoint = null) {

    $token    = is_null($token) ? variable_get('stanford_capx_token','') : $token;
    $endpoint = is_null($endpoint) ? CAPx::getAPIEndpoint() : $endpoint;

    $return = (object) array(
      'status' => 0,
      'message' => t('API connection failed'),
      'code' => "ERROR: ",
    );

    $client = new HTTPClient();
    $client->setEndpoint($endpoint);
    $client->setApiToken($token);

    try {
      $results = $client->api('search')->keyword('test');
    }
    catch(\Exception $e) {
      $return->message = $e->getMessage();
      return $return;
    }

    if (is_array($results)) {
      $return->status = 1;
      $return->message = t("connection successfull");
      $return->code = 200;
    }

    return $return;
  }

  /**
   * [renewConnectionToken description]
   * @return [type] [description]
   */
  public static function renewConnectionToken() {

    $username   = CAPx::getAuthUsername();
    $password   = CAPx::getAuthPassword();
    $authpoint  = CAPx::getAuthEndpoint();

    $client = new HTTPClient();
    $client->setEndpoint($authpoint);
    $response = $client->api('auth')->authenticate($username, $password);

    if ($response) {
      $token = $response->getAuthApiToken();
      variable_set('stanford_capx_token', $token);
      return TRUE;
    }

    throw new Exception("Could not authenticate with server.");
  }

  /**
   * Returns an authenticated HTTP Client for use.
   * @return HTTPClient an authenticated HTTP client ready to use.
   */
  public static function getAuthenticatedHTTPClient() {
    $username   = CAPx::getAuthUsername();
    $password   = CAPx::getAuthPassword();
    $token      = variable_get('stanford_capx_token', '');
    $endpoint   = CAPx::getAPIEndpoint();
    $authpoint  = CAPx::getAuthEndpoint();

    $connection = CAPxConnection::testConnectionToken($token);

    if (!$connection->value) {
      $client = new HTTPClient();
      $client->setEndpoint($authpoint);
      $response = $client->api('auth')->authenticate($username, $password);
      if ($response) {
        $token = $response->getAuthApiToken();
        variable_set('stanford_capx_token', $token);
      }
      else {
        throw new \Exception("Could not authenticate with API server.");
      }
    }

    $client->setApiToken($token);
    $client->setEndpoint($endpoint);
    return $client;
  }

}
