Drupal.behaviors.yourvariablehere = {
  attach: function(context, settings) {

  (function ($) {

    /**
     * [doHeight description]
     * @return {[type]} [description]
     */
    var doHeight = function() {
      var maxH = Math.round($(window).height() * 0.8);;
      var block = $("#block-stanford-capx-data-browser-launch");

      // If it is floating we can go a bit bigger.
      if (block.hasClass("is-floating")) {
        maxH = Math.round($(window).height() * 0.95);
      }

      block.css("height", maxH + "px");
      block.css("max-height", maxH + "px");
    };

    // Set this variable with the height of your sidebar + header.
    // ------------------------------------------------------------------------.
    var offsetPixels = 200;

    $(window).scroll(function() {
      if ($(window).scrollTop() > offsetPixels) {
        var block = $("#block-stanford-capx-data-browser-launch");
        block.css({
            "position": "relative",
            "top": Math.round($(window).scrollTop() - offsetPixels) + 30 + "px"
          });

        if (!block.hasClass("is-floating")) {
          block.addClass("is-floating");
        }
        // Resize the block.
        doHeight();
      }
      else {
        $("#block-stanford-capx-data-browser-launch").css({
          "position": "static"
        })
          .removeClass("is-floating");

        // Resize the block.
        doHeight();
      }
    });


    // Resize the height.
    // ------------------------------------------------------------------------.
    $(window).resize(function() {
      // Resize the block.
      doHeight();
    });


  })(jQuery);

  }
};

