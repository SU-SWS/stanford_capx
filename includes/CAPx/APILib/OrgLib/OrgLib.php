<?php
/**
 * @file
 */

namespace CAPx\APILib\OrgLib;
use CAPx\APILib\AbstractAPILib as APILib;

class OrgLib extends APILib {

    /**
     * [get description]
     * @param  [type] $type [description]
     * @param  [type] $params [description]
     * @return [type]       [description]
     */
    public function get($vars = null) {
      $endpoint = $this->get_endpoint();
      $options = $this->get_options();

      if (!is_array($vars)) {
        $endpoint .= "/api/cap/v1/orgs/" . $vars;
      }
      else {
        $endpoint .= "/api/cap/v1/orgs";
        $options['query']['orgCodes'] = implode(",", $vars);
      }

      return $this->make_request($endpoint, array(), $options);
    }

    /**
     * [get_profiles description]
     * @param  [type] $params [description]
     * @return [type]         [description]
     */
    public function get_profiles($params = null) {
      $endpoint = $this->get_endpoint();
      $endpoint .= "/api/cap/v1/orgs/" . $params . "/profiles";

      $options = $this->get_options();
      $params = array('path' => $params);

      return $this->make_request($endpoint, array(), $options);
    }


}
