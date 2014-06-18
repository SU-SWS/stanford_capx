<?php
/**
 * @file
 */

namespace CAPx\APILib\SchemaLib;
use CAPx\APILib\AbstractAPILib as APILib;

class SchemaLib extends APILib {

  /**
   * @param  string  $order    [description]
   * @return [type]            [description]
   */
  public function profile() {

    $endpoint = $this->get_endpoint() . "/api/cap/v1/schemas/profile";
    $options = $this->get_options();
    return $this->make_request($endpoint, array(), $options);

  }


}
