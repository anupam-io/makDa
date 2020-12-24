Forked from [Web-Crawler](https://github.com/ShrutiKatpara/Web-Crawler).
Visit the above link for detailed information about the project.

# makDa - A multithreaded web crawler with libcURL & thread-safe data structures
## Added features
 - Dowloading files with [**libcURL**](https://curl.se/libcurl/): One of the bugs in previous projects was crawler stopping at the end of its execution. In somecases `https` requests would not get any response and they would wait forever(very long time). Because of the use of `OpenSSL`, it was not possible to implement a time out functionality. By using *libcURL*, we can add the time out feature. Hurray! now crawler won't stop.
 - **Saving and resume working state**: Once crawler runs adn finishes its execution, it was dumping the data into output files. I have added the saving feature, now the state of the crawler gets saved in a `.zip` file. And at the time of initialization, it <ins>recovers its state</ins>.

## How to run makDa
 - use `make` to compile and run makDa, it would run with its default arguments.
 - supported arguments are:
   - `maxlinks`     : maximum links extracted from a single website
   - `pagelimit`		: maximum pages to process
   - `threads`			: maximum concurrent threads to create
   - `maxfilesize`	: maximum file size to download
   - `timeout`			: maximum time waiting for website response
   - `restore_data` : restoring crawler state from previous run
   - `save_data`		: saving current crawler state for next run

## Contributors
<table><tr><td align="center">
        <a href="https://github.com/akcgjc007">
            <img src="https://avatars2.githubusercontent.com/u/56300182" width="100;" alt="anupam"/>
            <br />
            <sub><b>Anupam Kumar</b></sub>
        </a>
    </td></tr>
</table>
