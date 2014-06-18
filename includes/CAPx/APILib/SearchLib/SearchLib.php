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

    $endpoint = $this->get_endpoint() . "/api/cap/v1/search/autocomplete";
    $options = $this->get_options();
    $options['query']['q'] = $string;

    return $this->make_request($endpoint, array(), $options);

  }


  /**
   * @param  string  $order    [description]
   * @return [type]            [description]
   */
  public function keyword($string = '') {

    $endpoint = $this->get_endpoint() . "/api/cap/v1/search/keyword";
    $options = $this->get_options();
    $options['query']['q'] = $string;

    return $this->make_request($endpoint, array(), $options);

  }

}
