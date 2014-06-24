<?php
/**
 * @file
 */

namespace CAPx\APILib\SearchLib;
use CAPx\APILib\AbstractAPILib as APILib;

class SearchLib extends APILib {

  /**
   * @param  string  $order    [description]
   * @return [type]            [description]
   */
  public function autocomplete($string = '') {

    $endpoint = $this->getEndpoint() . "/api/cap/v1/search/autocomplete";
    $options = $this->getOptions();
    $options['query']['q'] = $string;

    return $this->makeRequest($endpoint, array(), $options);

  }


  /**
   * @param  string  $order    [description]
   * @return [type]            [description]
   */
  public function keyword($string = '') {

    $endpoint = $this->getEndpoint() . "/api/cap/v1/search/keyword";
    $options = $this->getOptions();
    $options['query']['q'] = $string;

    return $this->makeRequest($endpoint, array(), $options);

  }

}
