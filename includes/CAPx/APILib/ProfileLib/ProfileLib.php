<?php
/**
 * @file
 */

namespace CAPx\APILib\ProfileLib;
use CAPx\APILib\AbstractAPILib as APILib;

class ProfileLib extends APILib {

  /**
   * [search description]
   * @param  [type]  $type     [description]
   * @param  [type]  $args     [description]
   * @param  boolean $exact    [description]
   * @param  boolean $children [description]
   * @param  string  $order    [description]
   * @return [type]            [description]
   */
  public function search($type, $args, $exact = FALSE, $children = FALSE, $order = '') {

    $endpoint = $this->getEndpoint() . "/api/profiles/v1";
    $options = $this->getOptions();

    switch($type) {
      case "ids":
      case "uids":
      case "universityIds":
      case "orgCodes":
      case "privGroups":
        $options['query'][$type] = implode(",", $args);
        break;
      case "name":
      case "orgAlias":
        $options['query'][$type] = $args;
        break;
      default:
        throw new Exception("Missing list type.");
    }

    $options['query']['exact'] = ($exact) ? "true" : "false";
    $options['query']['includeChildren'] = ($children) ? "true" : "false";
    if (!empty($order)) {
      $options['query']['order'] = $order;
    }

    return $this->makeRequest($endpoint, array(), $options);

  }

  /**
   * [get description]
   * @param  [type] $params [description]
   * @return [type]         [description]
   */
  public function get($profileId) {
    $endpoint = $this->getEndpoint() . "/api/profiles/v1/" . $profileId;
    $options = $this->getOptions();

    return $this->makeRequest($endpoint, array(), $options);

  }


}
