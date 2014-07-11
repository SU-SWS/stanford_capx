<?php
/**
 * @file
 * The SearchLib library supports results for an autocomplete search box and
 * a full keyword search. Search is performed by keyword and is typically used
 * with searching for profiles by name.
 *
 * EXAMPLES:
 * $client = new HTTPClient();
 * $autocomplete = $client->api('search')->autocomplete($string);
 *
 * $client = new HTTPClient();
 * $autocomplete = $client->api('search')->keyword($string);
 */

namespace CAPx\APILib\SearchLib;
use CAPx\APILib\AbstractAPILib as APILib;

class SearchLib extends APILib {

  /**
   * @param  string  $string   A string of text to search the cap api
   *                           profiles for.
   * @return mixed             Either an array of possible autocomplete
   *                           suggestions or false if something went wrong.
   */
  public function autocomplete($string = '') {
    $endpoint = $this->getEndpoint() . "/api/cap/v1/search/autocomplete";
    $options['query']['q'] = $string;

    return $this->makeRequest($endpoint, array(), $options);
  }


  /**
   * @param  string  $string   A string of text to search the cap api
   *                           profiles for.
   * @return mixed             Either an array of matches or false if something
   *                           went wrong.
   */
  public function keyword($string = '') {
    $endpoint = $this->getEndpoint() . "/api/cap/v1/search/keyword";
    $options['query']['q'] = $string;

    return $this->makeRequest($endpoint, array(), $options);
  }

}
