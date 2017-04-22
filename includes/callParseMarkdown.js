(function ($) {
  Drupal.behaviors.trpParseMarkdown = {
    attach: function (context, settings) {

      // Intialize the converted.
      var converter = new showdown.Converter();

      // Ensure underscores are not removed.
      converter.setOption('literalMidWordUnderscores', true);

      // Convert the content of .markdown-content.
      var html = converter.makeHtml( $('#content script.markdown-content').html()  );

      // Replace the original markdown with the resultant HTML.
      $('#content script.markdown-content').replaceWith(html);

    }
  };
}(jQuery));
