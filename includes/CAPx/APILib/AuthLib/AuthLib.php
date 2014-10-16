<?php
/**
 * @file
 * Authentication Library for connecting with the CAP API service. This class
 * is returned through the HTTPClient->api function when 'auth' is set. See
 * example below. When valid username and password credentials are passed
 * through the authenticate method the CAP API token is set. This token is used
 * to make calls to protected parts of the API and is passed along as a
 * parameter.
 *
 * EXAMPLE:
 * $client = new HTTPClient();
 * $response = $client->api('auth')->authenticate('xxx', 'xxx');
 * $token = $response->getAuthApiToken();
 *
 * For debugging you can get the last response via getLastResponse():
 *
 * $client = new HTTPClient();
 * $response = $client->api('auth')->authenticate('xxx', 'xxx');
 * $raw = $response->getLastResponse();
 */

namespace CAPx\APILib\AuthLib;

use CAPx\APILib\AbstractAPILib as APILib;
use \Guzzle\Http\Client as GuzzleClient;

class AuthLib extends APILib {

  // API OAuth Token (string).
  protected $authApiToken;
  // API Token Expires Epoc timestampe (int).
  protected $authApiTokenExpires;
  // Authentication endpoint default.
  // Can be set with $object->setEndpoint($endpoint);
  protected $endpoint = "https://authz.stanford.edu/oauth/token";
  // Authentication Parameters.
  protected $authParams = array('grant_type' => 'client_credentials');

  /**
   * Getter for authApiToken.
   * @return mixed
   *   Returns either false or a string after authenticate has been called.
   */
  public function getAuthApiToken() {
    return $this->authApiToken;
  }

  /**
   * Setter for authApiToken.
   *
   * This is really an internal function that gets called when the authenticate
   * method has been called. Could be used to update the token.
   *
   * @param string $token
   *   A very long string for use in making calls to parts of the api that are
   *   protected. False if authenticate fails.
   */
  public function setAuthApiToken($token) {
    $this->authApiToken = $token;
  }

  /**
   * Getter for authApiTokenExpires.
   *
   * Token expires is set after the authenticate method has been called.
   * Contains an integer on when the token that was received expires next and
   * will have to be re-authenticated.
   *
   * @return int
   *   epoc time for when the token expires.
   */
  public function getAuthApiTokenExpires() {
    return $this->authApiTokenExpires;
  }

  /**
   * Setter for authApiTokenExpires.
   *
   * This is mostly an internal function that is called after the authenticate method.
   *
   * @param int $time
   *   an integer for the epoc time when the last auth token
   * expires.
   */
  public function setAuthApiTokenExpires($time) {
    $this->authApiTokenExpires = $time;
  }

  /**
   * Setter for authParams.
   *
   * @param array $params
   *   an array of authentication params to set as a query parameters for the
   *   authenticate http request.
   */
  protected function setAuthParams($params) {
    $this->authParams = $params;
  }

  /**
   * Getter for authParams.
   *
   * @return array
   *   returns an array of authentication parameters for use in the authenticate
   *   method
   */
  protected function getAuthParams() {
    return $this->authParams;
  }

  /**
   * Authenticates a username and password with the CAP API.
   *
   * Response returns an API token and expires integer. The API token can then
   * be used in future API calls to protected parts of the API.
   *
   * @return bool
   *   True for success and false for some issue.
   */
  public function authenticate($username, $password) {

    // Get some things.
    $client = $this->getClient();
    $parameters = $this->getAuthParams();
    $endpoint = $this->getEndpoint();

    // Contact the server. Set exceptions == false so that Guzzle does not kill
    // everything if the server fails to return what we need.
    $request = $client->get($endpoint, array(), array('query' => $parameters, 'exceptions' => FALSE));

    // Set the username and password. Any will allow for a number of different
    // authentication methods and will automagically find the right one.
    $request->setAuth($username, $password, 'any');

    // Make the call and save the response.
    $response = $request->send();

    // Store the last response for later use.
    $this->setLastResponse($response);

    // Validate response code.
    $code = $response->getStatusCode();

    // @todo: handle non 200 responses with error logging.
    switch ($code) {
      case '200':
        try {
          $json = $response->json();
          $this->setAuthApiToken($json['access_token']);
          $this->setAuthApiTokenExpires($json['expires_in']);
        }
        catch(\Guzzle\Common\Exception\RuntimeException $e) {
          drupal_set_message('Could not parse json response. Please check endpoint configuration.', 'error', FALSE);
          return $this;
        }
        break;

      default:
        return $this;
      break;
    }

    return $this;

  }

}
