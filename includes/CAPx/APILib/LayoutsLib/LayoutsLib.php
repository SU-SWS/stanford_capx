<?php
/**
 * @file
 * The Layouts Library is used for communicating with the CAP API's layouts
 * endpoint. The layouts endpoint returns a json array of information about
 * the fields used with a particular profile type. A few helpful wrapper methods
 * are included.
 *
 * Example:
 * $client = new HTTPClient();
 * $staff = $client->api('layouts')->staff();
 * $faculty = $client->api('layouts')->faculty();
 * $other = $client->api('layouts')->getType('other');
 *
 */

namespace CAPx\APILib\LayoutsLib;
use CAPx\APILib\AbstractAPILib as APILib;

class LayoutsLib extends APILib {

  /**
   * Wrapper for getType(faculty)
   * @return false or an array of layout data
   */
  public function faculty() {
    return $this->getType('faculty');
  }

  /**
   * Wrapper for getType(staff)
   * @return false or an array of layout data
   */
  public function staff() {
    return $this->getType('staff');
  }

  /**
   * Requests layout information from the CAP API layouts endpoint by type.
   * @param  string $type the type of profile. eg: staff
   * @return mixed false or an array of layout data
   */
  public function getType($type) {
    $endpoint = $this->getEndpoint() . "/api/cap/v1/layouts/" . $type;
    return $this->makeRequest($endpoint);
  }


}
