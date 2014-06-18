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
  protected $auth_api_token;
  // API Token Expires
  protected $auth_api_token_expires;
  // Authentication endpoint
  protected $endpoint = "https://authz.stanford.edu/oauth/token";
  // Authentication Parameters
  protected $auth_params = array('grant_type' => 'client_credentials');

  /**
   * [get_auth_api_token description]
   * @return [type] [description]
   */
  public function get_auth_api_token() {
    return $this->auth_api_token;
  }

  /**
   * [set_auth_api_token description]
   */
  public function set_auth_api_token($token) {
    $this->auth_api_token = $token;
  }

  /**
   * [get_auth_api_token description]
   * @return [type] [description]
   */
  public function get_auth_api_token_expires() {
    return $this->auth_api_token_expires;
  }

  /**
   * [set_auth_api_token description]
   */
  public function set_auth_api_token_expires($time) {
    $this->auth_api_token_expires = $time;
  }

  /**
   * [set_auth_params description]
   * @param [type] $params [description]
   */
  protected function set_auth_params($params) {
    $this->auth_params = $params;
  }

  /**
   * [get_auth_params description]
   * @return [type] [description]
   */
  protected function get_auth_params() {
    return $this->auth_params;
  }

  /**
   * [authenticate description]
   * @return [type] [description]
   */
  public function authenticate($username, $password) {

    // Get the guzzle client.
    $client = $this->get_client();

    // Get som additional parameters.
    $parameters = $this->get_auth_params();
    $endpoint = $this->get_endpoint();

    // Contact the server.
    $request = $client->get($this->get_endpoint(), array(), array('query' => $parameters, 'exceptions' => FALSE));
    $request->setAuth($username, $password, 'Any');

    $response = $request->send();

    $code = $response->getStatusCode();
    switch ($code) {
      case '200':
        $json = $response->json();
        $this->set_auth_api_token($json['access_token']);
        $this->set_auth_api_token_expires($json['expires_in']);
        break;

      default:
        return FALSE;
        break;
    }

    return $this;

  }



}
