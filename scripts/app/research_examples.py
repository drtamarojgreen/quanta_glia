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
    },
    {
        "name": "Negation Checking",
        "research_topic": "Describe the benefits of static typing in Python without mentioning JavaScript.",
        "evaluation_points": [
            {"text": "The answer should not mention 'JavaScript' or 'JS'.",
             "category": "Constraint",
             "weight": 2.0,
             "type": "negation",
             "params": {"keywords": ["JavaScript", "JS"]}},
            {"text": "The answer mentions benefits like 'readability' or 'errors'.",
             "category": "Completeness",
             "weight": 1.0,
             "type": "keyword",
             "params": {"keywords": ["readability", "errors", "bugs", "maintainability"], "min_count": 2}}
        ],
        "answers": [
            "Static typing helps catch bugs early and improves code readability. It makes large projects more maintainable.",
            "Static typing is great because it's not like JavaScript where types are loose. This helps prevent errors.",
            "Using type hints in Python makes the code self-documenting."
        ],
        "evaluation_type": "comparison"
    },
    {
        "name": "Golden Answer Diffing",
        "research_topic": "Explain the process of photosynthesis in simple terms.",
        "evaluation_points": [
            {
                "text": "The answer should be very similar to the golden answer.",
                "category": "Fidelity",
                "weight": 1.0,
                "type": "diff",
                "params": {
                    "golden_answer": "Photosynthesis is the process used by plants, algae, and some bacteria to convert light energy into chemical energy, through a process that converts carbon dioxide and water into glucose and oxygen.",
                    "threshold": 0.85
                }
            }
        ],
        "answers": [
            # This one is very close and should pass
            "Photosynthesis is the process used by plants and other organisms to convert light energy into chemical energy. It converts carbon dioxide and water into glucose and oxygen.",
            # This one is missing key parts and should fail
            "Photosynthesis is how plants make food using sunlight.",
            # This one is also close but might be on the edge
            "Photosynthesis is a process for plants to turn light into energy. They use water and carbon dioxide to make sugar and oxygen."
        ],
        "evaluation_type": "individual"
    },
    {
        "name": "Conditional Evaluation",
        "research_topic": "Provide a Python function that adds two numbers.",
        "evaluation_points": [
            {
                "id": "has-code-block",
                "text": "The answer must contain a Python code block.",
                "category": "Format",
                "weight": 2.0,
                "type": "regex",
                "params": {"pattern": "```python(.*?)```"}
            },
            {
                "id": "has-def-keyword",
                "text": "The code block must contain the 'def' keyword.",
                "category": "Syntax",
                "weight": 1.0,
                "type": "keyword",
                "params": {"keywords": ["def"]},
                "depends_on": "has-code-block"
            }
        ],
        "answers": [
            # This should pass both
            "Here is the function:\n```python\ndef add(a, b):\n  return a + b\n```",
            # This should fail the first check and skip the second
            "The function is `def add(a, b): return a + b`",
            # This should pass the first check but fail the second
            "Here is the code:\n```python\n# A function to add two numbers\nresult = 5 + 3\n```"
        ],
        "evaluation_type": "individual"
    },
    {
        "name": "Advanced Reporting Features",
        "research_topic": "What is the capital of France?",
        "evaluation_points": [
            {
                "text": "The answer must contain 'Paris'.",
                "category": "Correctness",
                "weight": 1.0,
                "type": "keyword",
                "params": {"keywords": ["Paris"]}
            },
            {
                "text": "Log the word count for analysis.",
                "category": "Metadata",
                "informational": True,
                "type": "length",
                "params": {}
            }
        ],
        "answers": [
            "The capital of France is Paris.",
            "France's capital city is Lyon."
        ],
        "evaluation_type": "individual"
    }
]
