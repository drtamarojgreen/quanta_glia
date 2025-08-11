EXAMPLES = [
    {
        "name": "Technical Explanation",
        "research_topic": "Explain the concept of TissLang and give two code examples.",
        "evaluation_points": [
            {"text": "The answer clearly defines TissLang's purpose.",
             "type": "keyword",
             "params": {"keywords": ["TissLang", "purpose", "language"], "min_count": 3}},
            {"text": "The answer provides at least two distinct code examples.",
             "type": "regex",
             "params": {"pattern": r"```(python|tisslang).*?```.*?```(python|tisslang).*?```"}},
            {"text": "The answer is at least 50 words long.",
             "type": "length",
             "params": {"min": 50}},
            {"text": "The explanation is semantically similar to the canonical definition.",
             "type": "embedding",
             "params": {"target": "TissLang is a domain-specific language for defining tissue simulations.", "threshold": 0.7}}
        ],
        "answers": [
            "TissLang is a special language for tissue simulation. Here is one example: ```python\nprint('hello')\n``` and another ```python\nprint('world')\n```. Its purpose is clear.",
            "TissLang is a domain-specific language designed from the ground up for defining complex tissue simulations and cellular interactions. The purpose of the language is to provide a clear and concise syntax. Here are two code blocks: ```tisslang\ninit_grid(10,10)\n``` and ```tisslang\nadd_cells(5)\n```"
        ],
        "evaluation_type": "comparison"
    },
    {
        "name": "JSON Schema Validation",
        "research_topic": "Provide user data in JSON format for user_id 123.",
        "evaluation_points": [
            {"text": "The output is valid JSON that conforms to the user schema.",
             "type": "json_schema",
             "params": {"schema": {
                 "type": "object",
                 "properties": {
                     "user_id": {"type": "number"},
                     "user_name": {"type": "string"},
                     "is_active": {"type": "boolean"},
                 },
                 "required": ["user_id", "user_name", "is_active"]
             }}},
        ],
        "answers": [
            "{'user_id': 123, 'user_name': 'test', 'is_active': true}",
            '{"user_id": 123, "user_name": "test"}',
            '{"user_id": 123, "user_name": "test", "is_active": true}'
        ],
        "evaluation_type": "individual"
    },
    {
        "name": "Citation Checking",
        "research_topic": "Summarize the findings of the paper 'Attention is All You Need' and cite your sources.",
        "evaluation_points": [
            {"text": "The answer contains at least 2 citations in the format [n].",
             "type": "citation",
             "params": {"pattern": r'\[\d+\]', "min_count": 2}},
            {"text": "The answer mentions the 'Transformer' model.",
             "type": "keyword",
             "params": {"keywords": ["Transformer"], "min_count": 1}}
        ],
        "answers": [
            "The paper introduced the Transformer model, which is very important.",
            "The paper 'Attention is All You Need' introduced the Transformer model [1]. It relies on self-attention mechanisms.",
            "The paper introduced the Transformer model, a novel network architecture based solely on attention mechanisms [1]. It dispenses with recurrence and convolutions entirely [2]."
        ],
        "evaluation_type": "comparison"
    },
    {
        "name": "Code Execution Validation",
        "research_topic": "Write a Python script that prints 'Hello, World!'.",
        "evaluation_points": [
            {"text": "The script should print 'Hello, World!' to stdout.",
             "type": "code_test",
             "params": {"stdout": "Hello, World!"}},
            {"text": "The script should exit with code 0.",
             "type": "code_test",
             "params": {"exit_code": 0}}
        ],
        "answers": [
            "Here is the script:\n```python\nprint('Hello, World!')\n```",
            "Of course:\n```python\nprint('Hello World')\n```",
            "Certainly:\n```python\nprint(hello)\n```"
        ],
        "evaluation_type": "individual"
    }
]
