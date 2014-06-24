<?php
/**
 * @file
 * Authentication Library for connecting with the CAP API service
 */

namespace CAPx\APILib\AuthLib;

use CAPx\APILib\AbstractAPILib as APILib;
use \Guzzle\Http\Client as GuzzleClient;

class AuthLib extends APILib {

  // API OAuth Token
  protected $authApiToken;
  // API Token Expires
  protected $authApiTokenExpires;
  // Authentication endpoint
  protected $endpoint = "https://authz.stanford.edu/oauth/token";
  // Authentication Parameters
  protected $authParams = array('grant_type' => 'client_credentials');

  /**
   * [getAuthApiToken description]
   * @return [type] [description]
   */
  public function getAuthApiToken() {
    return $this->authApiToken;
  }

  /**
   * [setAuthApiToken description]
   */
  public function setAuthApiToken($token) {
    $this->authApiToken = $token;
  }

  /**
   * [getAuthApiToken description]
   * @return [type] [description]
   */
  public function getAuthApiTokenExpires() {
    return $this->authApiTokenExpires;
  }

  /**
   * [setAuthApiToken description]
   */
  public function setAuthApiTokenExpires($time) {
    $this->authApiTokenExpires = $time;
  }

  /**
   * [setAuthParams description]
   * @param [type] $params [description]
   */
  protected function setAuthParams($params) {
    $this->authParams = $params;
  }

  /**
   * [getAuthParams description]
   * @return [type] [description]
   */
  protected function getAuthParams() {
    return $this->authParams;
  }

  /**
   * [authenticate description]
   * @return [type] [description]
   */
  public function authenticate($username, $password) {

  // Get some
    $client = $this->getClient();
    $parameters = $this->getAuthParams();
    $endpoint = $this->getEndpoint();

    // Contact the server.
    $request = $client->get($this->getEndpoint(), array(), array('query' => $parameters, 'exceptions' => FALSE));
    $request->setAuth($username, $password, 'any');

    $response = $request->send();

    $code = $response->getStatusCode();
    switch ($code) {
      case '200':
        $json = $response->json();
        $this->setAuthApiToken($json['access_token']);
        $this->setAuthApiTokenExpires($json['expires_in']);
        break;

      default:
        return FALSE;
        break;
    }

    return $this;

  }



}
