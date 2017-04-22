(function ($) {
  Drupal.behaviors.trpParseMarkdown = {
    attach: function (context, settings) {

      // Intialize the converted.
      var converter = new showdown.Converter();

      // Convert the content of .markdown-content.
      var html = converter.makeHtml( $('#content div.markdown-content').html()  );

      // Replace the original markdown with the resultant HTML.
      $('#content div.markdown-content').replaceWith(html);

    }
  };
}(jQuery));
