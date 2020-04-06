void make_html_friendly(unsigned char * string, char * html_version) {
    /* Converts text to use HTML entity codes */

    int i, html_pos;

    html_pos = 0;
    html_version[html_pos] = '\0';

    for (i = 0; i < (int) ustrlen(string); i++) {
        switch(string[i]) {
            case '>':
                strcat(html_version, "&gt;");
                html_pos += 4;
                break;

            case '<':
                strcat(html_version, "&lt;");
                html_pos += 4;
                break;

            case '&':
                strcat(html_version, "&amp;");
                html_pos += 5;
                break;

            case '"':
                strcat(html_version, "&quot;");
                html_pos += 6;
                break;

            case '\'':
                strcat(html_version, "&apos;");
                html_pos += 6;
                break;

            default:
                html_version[html_pos] = string[i];
                html_pos++;
                html_version[html_pos] = '\0';
                break;
         }
    }
}
