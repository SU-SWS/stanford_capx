<?php
/**
 * @file
 */

namespace CAPx\APILib\LayoutsLib;
use CAPx\APILib\AbstractAPILib as APILib;

class LayoutsLib extends APILib {

  /**
   * Wrapper for getType(faculty)
   * @return [type] [description]
   */
  public function faculty() {
    return $this->getType('faculty');
  }

  /**
   * Wrapper for getType(staff)
   * @return [type] [description]
   */
  public function staff() {
    return $this->getType('staff');
  }

  /**
   * [getType description]
   * @param  [type] $type [description]
   * @return [type]       [description]
   */
  public function getType($type) {
    $endpoint = $this->getEndpoint() . "/api/cap/v1/layouts/" . $type;
    $options = $this->getOptions();
    return $this->makeRequest($endpoint, array(), $options);
  }


}
