<?php
/**
 * @file
 */

namespace CAPx\APILib\LayoutsLib;
use CAPx\APILib\AbstractAPILib as APILib;

class LayoutsLib extends APILib {

  /**
   * Wrapper for get_type(faculty)
   * @return [type] [description]
   */
  public function faculty() {
    return $this->get_type('faculty');
  }

  /**
   * Wrapper for get_type(staff)
   * @return [type] [description]
   */
  public function staff() {
    return $this->get_type('faculty');
  }

  /**
   * [get_type description]
   * @param  [type] $type [description]
   * @return [type]       [description]
   */
  public function get_type($type) {
    $endpoint = $this->get_endpoint() . "/api/cap/v1/layouts/" . $type;
    $options = $this->get_options();
    return $this->make_request($endpoint, array(), $options);
  }


}
