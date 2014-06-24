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
      $endpoint = $this->getEndpoint();
      $options = $this->getOptions();

      if (!is_array($vars)) {
        $endpoint .= "/api/cap/v1/orgs/" . $vars;
      }
      else {
        $endpoint .= "/api/cap/v1/orgs";
        $options['query']['orgCodes'] = implode(",", $vars);
      }

      return $this->makeRequest($endpoint, array(), $options);
    }

    /**
     * [getProfiles description]
     * @param  [type] $params [description]
     * @return [type]         [description]
     */
    public function getProfiles($params = null) {
      $endpoint = $this->getEndpoint();
      $endpoint .= "/api/cap/v1/orgs/" . $params . "/profiles";

      $options = $this->getOptions();
      $params = array('path' => $params);

      return $this->makeRequest($endpoint, array(), $options);
    }


}
