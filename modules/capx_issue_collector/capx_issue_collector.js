/**
 * Javascript issue collector.
 */

(function ($) {
  Drupal.behaviors.capx_issue_collector = {
    attach: function (context, settings) {

      $.ajax({
        url: "https://stanfordits.atlassian.net/s/c64c585d847e1c97eda4c6f4b2a5ca4d-T/en_US879it1/64005/66/1.4.17/_/download/batch/com.atlassian.jira.collector.plugin.jira-issue-collector-plugin:issuecollector-embededjs/com.atlassian.jira.collector.plugin.jira-issue-collector-plugin:issuecollector-embededjs.js?locale=en-US&collectorId=ca75a831",
        type: "get",
        cache: true,
        dataType: "script"
      });

    }
  };

})(jQuery);
