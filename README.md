# A tiny publishing platform in C

### Dependencies
* libevhtp
* openssl

### Configuration

Create a file in `resources/config.txt` following this template:

```
(conf 
    (title "My articles")
    (base-url "https://my.articles.me/")
    (links (("mail" "my@email.me") ("stuff" "https://somewhere.else")))
```

### Adding articles

Articles must be placed in `resources/articles` and have the following structure:

```
29/08/2018 21:30
How not to be seen

Some something<br />
<h1>Something else</h1>
```

Where, in order:

*Line 1* – publishing date and time formatted as `%d/%m/%Y %H:%M`  
*Line 2* – title  
*Rest* – HTML content
