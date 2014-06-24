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

    $endpoint = $this->getEndpoint() . "/api/cap/v1/schemas/profile";
    $options = $this->getOptions();
    return $this->makeRequest($endpoint, array(), $options);

  }


}
